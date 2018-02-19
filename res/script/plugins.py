import os as _os
import re as _re

plugins = {}


def init(configs):
    def scripts(configs_):
        for config in configs_:
            if _os.path.exists(config):
                for file in _os.listdir(config):
                    path = _os.path.join(config, file)
                    if file.endswith('.py') and _os.path.isfile(path):
                        yield path

    plugins['Dummy'] = LoaderDummy()
    plugins['MD2D'] = LoaderMd2d()
    for script in scripts(configs):
        variables = {}
        with open(script) as s:
            # noinspection PyBroadException
            try:
                exec(s.read(), variables)
                plugin = variables['plugin']
                if 'name' and 'load' in dir(plugin):
                    plugins[plugin.name()] = plugin
                else:
                    print('Failed to load file "' +
                          script + '": attribute not met')
            except Exception:
                print('Failed to load file "' +
                      script + '": exception caught.')

    ret = [i for i in plugins.keys()]
    return ret


def load(name, files):
    plugin = plugins.get(name)
    if plugin is None:
        return "Plugin not found."
    else:
        return plugin.load(files)


class LoaderDummy:
    @staticmethod
    def name():
        return "Dummy"

    # noinspection PyUnusedLocal
    @staticmethod
    def load(files):
        return [{
            'abbr': 'Rea-1',
            'name': 'Reaction A + B <=> C',
            'children': [],
            'quantities': [{
                'name': 'example',
                'times': [1, 2, 3],
                'dimData': 1,
                'sizeData': [3, 2],
                'sizeModel': [[2, 3], [4, 5]],
                'data': lambda: [
                    [0, 1, 0, 0, 0, 0],
                    [1, 1, 1, 2, 0, 0],
                    [0, 2, 0, 0, 0, 0]
                ]
            }]
        }]


class LoaderMd2d:
    pat_split = _re.compile(r' +')
    pat_time = _re.compile(r'# *time *= *(.+?)\s')
    pat_data = _re.compile(r'( +\S+)+?')
    pat_comp = _re.compile(r'# *(\w)-components')
    pat_par = _re.compile(r'n\s+(\S+)\s+\S+\s+\S+')
    pat_rea = _re.compile(r'R\d\s{2,}(\S.+\S)\s{2,}\S+\s{2,}\S+')

    @staticmethod
    def name():
        return "MD2D"

    @staticmethod
    def load(files):

        def find_name0(name):
            for file in files:
                if file.endswith(name):
                    return file
            raise FileNotFoundError('No file found for name: ' + name)

        def find_name1(name, index):
            return find_name0('{:}{:02d}.txt'.format(name, index))

        def gen_time():
            f = find_name0('D00.txt')
            return LoaderMd2d.read_time(f)

        def gen_qtt(name, file):
            dim, sx, sy = LoaderMd2d.read_dim(file)
            return {
                'name': name,
                'times': times,
                'dimData': dim,
                'sizeData': [sx, sy],
                'sizeModel': size_model,
                'data': lambda: LoaderMd2d.read_data(file)
            }

        def gen_par(name, index):
            qtt = [
                gen_qtt('D', find_name1('D', index)),
                gen_qtt('mu', find_name1('mu', index)),
                gen_qtt('n', find_name1('n', index)),
                gen_qtt('phi', find_name1('phi', index))
            ]
            return {
                'abbr': 'Par' + str(index),
                'name': name,
                'children': [],
                'quantities': qtt
            }

        def gen_rea(name, index):
            qtt = [
                gen_qtt('K', find_name1('K', index)),
                gen_qtt('R', find_name1('R', index))
            ]
            return {
                'abbr': 'Rea' + str(index),
                'name': name,
                'children': [],
                'quantities': qtt
            }

        def gen_ov(file):
            def item(name):
                return {
                    'name': name,
                    'times': t,
                    'dimData': 1,
                    'sizeData': [],
                    'sizeModel': [],
                    'data': lambda: cache[name]
                }

            with open(file) as f:
                s = f.readline()
                names = LoaderMd2d.pat_split.split(s)[:-1]
                data = [[] for i in names]
                for s in f:
                    ns = LoaderMd2d.pat_split.split(s)[:-1]
                    for i, n in enumerate(ns):
                        data[i].append([float(n)])
                for i, n in enumerate(names):
                    cache[n] = data[i]

            t = [i[0] for i in cache['t']]
            return [item(n) for n in names if n != 't']

        particles, reactions = LoaderMd2d.read_info(files)
        size_model = [[0, 1], [2, 3]]
        times = gen_time()
        cache = {}

        return [{
            'abbr': 'Par',
            'name': 'Particles',
            'children': [gen_par(j, i+1) for i, j in enumerate(particles)],
            'quantities': []
        }, {
            'abbr': 'Rea',
            'name': 'Reactions',
            'children': [gen_rea(j, i) for i, j in enumerate(reactions)],
            'quantities': []
        }, {
            'abbr': 'Ov',
            'name': 'Overview',
            'children': [],
            'quantities': gen_ov(find_name0('history.out'))
        }]

    @staticmethod
    def read_info(files):
        data = []
        reactions = []
        particles = []
        for file in files:
            if file.endswith('info.txt'):
                with open(file) as f:
                    s = f.readline()
                    start = s.split(' ', 1)[0]
                    data.append(s)
                    s = f.readline()
                    while s and not s.startswith(start):
                        if s and not s.isspace():
                            data.append(s)
                        s = f.readline()
                break
        for key in data:
            m = LoaderMd2d.pat_par.match(key)
            if m:
                particles.append(m.group(1))
                continue
            m = LoaderMd2d.pat_rea.match(key)
            if m:
                reactions.append(m.group(1))
        return particles, reactions

    @staticmethod
    def read_dim(file):
        def width(s_):
            nums = LoaderMd2d.pat_split.split(s_[:-1])
            return len(nums) - 1

        times = []
        two_d = False
        size_x = 0
        size_y = 0
        with open(file) as f:
            progress = 0  # 0 start, 1 time, 2 component, 3 data
            for s in f:
                if progress == 0:
                    m = LoaderMd2d.pat_time.match(s)
                    if m:
                        progress = 1
                        times.append(float(m.group(1)))
                elif progress == 1:
                    if LoaderMd2d.pat_comp.match(s):
                        two_d = True
                        progress = 2
                    elif LoaderMd2d.pat_data.match(s):
                        size_x = width(s)
                        size_y += 1
                        progress = 3
                elif progress == 2:
                    if LoaderMd2d.pat_data.match(s):
                        size_x = width(s)
                        size_y += 1
                        progress = 3
                elif progress == 3:
                    if LoaderMd2d.pat_data.match(s):
                        size_y += 1
                    else:
                        break

        return 2 if two_d else 1, size_x, size_y

    @staticmethod
    def read_time(file):
        ret = []
        with open(file) as f:
            for s in f:
                m = LoaderMd2d.pat_time.match(s)
                if m:
                    ret.append(float(m.group(1)))
        return ret

    @staticmethod
    def read_data(file):
        def convert(s_):
            slices = LoaderMd2d.pat_split.split(s_[:-1])
            slices = slices[1:]
            return [float(i) for i in slices]

        with open(file) as f:
            active = False
            data = []
            buf = []
            for s in f:
                if not active:
                    if LoaderMd2d.pat_data.match(s):
                        active = True
                        buf += convert(s)
                else:
                    if LoaderMd2d.pat_split.match(s):
                        buf += convert(s)
                    else:
                        active = False
                        data.append(buf)
                        buf = []
        return data

# if __name__ == "__main__":
#     d = '/run/media/towdium/Files/Work/FYP/software/data'
#     files_ = [_os.path.join(d, i) for i in _os.listdir(d)]
#     ret = LoaderMd2d().load(files_)
#     print(ret)
