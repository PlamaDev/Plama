import os as _os
import re as _re
import traceback as _tb
import itertools as _it
import typing as _tp


class Manager:
    types = {_tp.List[str]: 1}

    def __init__(self):
        self.plugins = {}

    def init(self, configs):
        def scripts_():
            for config in configs:
                if _os.path.exists(config):
                    for file in _os.listdir(config):
                        path = _os.path.join(config, file)
                        if file.endswith('.py') and _os.path.isfile(path):
                            yield path

        for i in [LoaderDummy(), LoaderMd2d(), LoaderError()]:
            self.plugins[i.name()] = (i.load, [(str(i), Manager.types[j]) for i, j in i.args()])

        for script in scripts_():
            variables = {}
            with open(script) as s:
                # noinspection PyBroadException
                try:
                    exec(s.read(), variables)
                    plugin = variables['plugin']
                    if 'name' and 'load' and 'args' in dir(plugin):
                        self.plugins[plugin.name()] = (
                            plugin.load, [(str(i), Manager.types[j]) for i, j in plugin.args()])
                    else:
                        print('Failed to load file "' +
                              script + '": attribute not met')
                except Exception:
                    print('Failed to load file "' +
                          script + '": exception caught.')

        ret = [i for i in self.plugins.keys()]
        return ret

    def args(self, name):
        return self.plugins[name][1]

    def load(self, name, arguments):
        func = self.plugins.get(name)[0]
        try:
            data = func(*arguments)
            ret = [Manager.check_node(j, '')
                   for i, j in enumerate(data)]
            return ret, None
        except Exception as e:
            _tb.print_exc()
            return None, str(e)

    @staticmethod
    def check_quantity(qtt, path):
        def chk_size_(size_):
            if len(size_) != 2:
                s = 'Quantity {}/{} has incorrectly formatted ' + \
                    'model size, found {:d}, expecting {:d}.'
                raise ValueError(s.format(path, ret['name'], len(size_), 2))
            else:
                return [float(i) for i in size_]

        def chk_data_(_data):
            ret_ = []
            try:
                _data = _data()
                if len(_data) != len(ret['times']) * ret['dimData']:
                    s = 'Data of quantity {}/{} has incorrect  ' + \
                        'section amount, find {:d} expecting {:d}.'
                    raise ValueError(s.format(path, ret['name'], len(
                        _data), len(ret['times']) * ret['dimData']))
                size = 1
                for j in ret['sizeData']:
                    size *= j
                for n, j in enumerate(_data):
                    if len(j) != size:
                        s = 'Data section {:d} of quantity {}/{} ' + \
                            'has incorrect size, find {:d} expecting {:d}.'
                        raise ValueError(
                            s.format(n, path, ret['name'], len(j), size))
                    ret_.append([float(k) for k in j])
                return ret_, None
            except Exception as e:
                _tb.print_exc()
                return None, str(e)

        ret = {
            'name': str(qtt['name']),
            'times': [float(i) for i in qtt['times']],
            'dimData': int(qtt['dimData']),
            'sizeData': [int(i) for i in qtt['sizeData']],
            'sizeModel': [chk_size_(i) for i in qtt['sizeModel']],
            'data': lambda: chk_data_(qtt['data'])
        }
        return ret

    @staticmethod
    def check_node(node, path):
        name = str(node['name'])
        p = '{}/{}'.format(path, name)
        ret = {
            'abbr': str(node['abbr']),
            'name': name,
            'children': [Manager.check_node(n, p) for n in node['children']],
            'quantities': [Manager.check_quantity(q, p) for q in node['quantities']]
        }
        return ret


class LoaderError:
    @staticmethod
    def name():
        return 'Error'

    @staticmethod
    def load(files):
        raise NotImplementedError('This is a exception test')

    @staticmethod
    def args():
        return []


class LoaderDummy:
    @staticmethod
    def name():
        return 'Dummy'

    # noinspection PyUnusedLocal
    @staticmethod
    def load():
        return [{
            'abbr': 'Rea-1',
            'name': 'Reaction A + B <=> C',
            'children': [],
            'quantities': [{
                'name': 'example-2D1D',
                'times': [1, 2, 3],
                'dimData': 1,
                'sizeData': [3, 3],
                'sizeModel': [[2, 3], [4, 5]],
                'data': lambda: [
                    [0, 1, 0, 0, 0, 0, 0, 0, 0],
                    [0, 0, 1, 0, 0, 0, 0, 0, 0],
                    [0, 2, 0, 0, 0, 0, 0, 0, 0]
                ]
            }, {
                'name': 'example-0D1D',
                'times': [1, 2, 3],
                'dimData': 1,
                'sizeData': [],
                'sizeModel': [],
                'data': lambda: [
                    [0], [3], [2]
                ]
            }, {
                'name': 'example-2D2D',
                'times': [1],
                'dimData': 2,
                'sizeData': [3, 2],
                'sizeModel': [[2, 3], [4, 5]],
                'data': lambda: [
                    [0, 1, 0, 0, 0, 0],
                    [0, 0, 1, 0, 0, 0]
                ]
            }, {
                'name': 'example-err',
                'times': [1, 2, 3],
                'dimData': 1,
                'sizeData': [],
                'sizeModel': [],
                'data': lambda: [[1], [2]]
            }]
        }]

    @staticmethod
    def args():
        return []


class LoaderMd2d:
    pat_split = _re.compile(r' +')
    pat_time = _re.compile(r'# *time *= *(.+?)\s')
    pat_data = _re.compile(r'( +\S+)+?')
    pat_comp = _re.compile(r'# *(\w)-components')
    pat_par = _re.compile(r'n\s+(\S+)\s+\S+\s+\S+')
    pat_rea = _re.compile(r'R\d\s{2,}(\S.+\S)\s{2,}\S+\s{2,}\S+')
    pat_dlt_x = _re.compile(r'\s*deltaX\s*(.+)\*(\D+?)\s*\n')
    pat_dlt_y = _re.compile(r'\s*deltaY\s*(.+)\*(\D+?)\s*\n')

    @staticmethod
    def name():
        return "MD2D"

    @staticmethod
    def load(output, sim):

        def find_name0_(name):
            for file in output:
                if file.endswith(name):
                    return file
            raise FileNotFoundError('No file found for name: ' + name)

        def find_name1_(name, index):
            return find_name0_('{:}{:02d}.txt'.format(name, index))

        def gen_time_():
            f = find_name0_('D00.txt')
            return LoaderMd2d.read_time(f)

        def gen_qtt_(name, file):
            dim, sx, sy = LoaderMd2d.read_dim(file)
            return {
                'name': name,
                'times': times,
                'dimData': dim,
                'sizeData': [sx, sy],
                'sizeModel': size_model,
                'data': lambda: LoaderMd2d.read_data(file)
            }

        def gen_par_(name, index):
            qtt = []
            for i in ['D', 'mu', 'n', 'phi']:
                try:
                    qtt.append(gen_qtt_(i, find_name1_(i, index)))
                except FileNotFoundError:
                    pass

            return {
                'abbr': 'Par' + str(index),
                'name': name,
                'children': [],
                'quantities': qtt
            }

        def gen_rea_(name, index):
            qtt = []
            for i in ['K', 'R']:
                try:
                    qtt.append(gen_qtt_(i, find_name1_(i, index)))
                except FileNotFoundError:
                    pass

            return {
                'abbr': 'Rea' + str(index),
                'name': name,
                'children': [],
                'quantities': qtt
            }

        def gen_ov():
            def item(name):
                return {
                    'name': name,
                    'times': t,
                    'dimData': 1,
                    'sizeData': [],
                    'sizeModel': [],
                    'data': lambda: cache[name]
                }

            with open(find_name0_('history.out')) as f:
                s = f.readline()
                names = LoaderMd2d.pat_split.split(s)[:-1]
                data = [[] for _ in names]
                for s in f:
                    ns = LoaderMd2d.pat_split.split(s)[:-1]
                    for i, n in enumerate(ns):
                        data[i].append([float(n)])
                for i, n in enumerate(names):
                    cache[n] = data[i]

            t = [i[0] for i in cache['t']]
            return [item(n) for n in names if n != 't']

        particles, reactions = LoaderMd2d.read_info(output)
        size_model = LoaderMd2d.read_model(sim)
        size_data = LoaderMd2d.read_dim(find_name0_('P.txt'))
        size_model = [[0, size_model[i] * size_data[i + 1]] for i in range(2)]
        fields = ['E', 'E_N', 'epsilon', 'Er', 'Ereff', 'J', 'Sigma', 'V']
        times = gen_time_()
        cache = {}

        return [{
            'abbr': 'Par',
            'name': 'Particles',
            'children': [gen_par_(j, i + 1) for i, j in enumerate(particles)],
            'quantities': []
        }, {
            'abbr': 'Rea',
            'name': 'Reactions',
            'children': [gen_rea_(j, i) for i, j in enumerate(reactions)],
            'quantities': []
        }, {
            'abbr': 'Ov',
            'name': 'Overview',
            'children': [],
            'quantities': gen_ov()
        }, {
            'abbr': 'Gnr',
            'name': 'General',
            'children': [],
            'quantities': [gen_qtt_(i, find_name0_(i + '.txt')) for i in fields]
        }]

    @staticmethod
    def args():
        return [('Output files', _tp.List[str]), ('Simulation file', _tp.List[str])]

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
        def width(string):
            numbers = LoaderMd2d.pat_split.split(string[:-1])
            return len(numbers) - 1

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

        return (2, size_x, size_y - 1) if two_d else (1, size_x, size_y)

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
    def read_model(file):
        nx = ux = ny = uy = None
        unit = {'m': 1, 'mm': 0.001}
        with open(file[0]) as f:
            for line in f:
                x = LoaderMd2d.pat_dlt_x.match(line)
                y = LoaderMd2d.pat_dlt_y.match(line)
                if x:
                    nx = float(x.group(1))
                    ux = x.group(2)
                elif y:
                    ny = float(y.group(1))
                    uy = y.group(2)
                if None not in [nx, ux, ny, uy]:
                    break
        return unit[ux] * nx, unit[uy] * ny

    @staticmethod
    def read_data(file):
        def convert_(string):
            slices = LoaderMd2d.pat_split.split(string[:-1])
            slices = slices[1:]
            return [float(i) for i in slices]

        def average_(d, is_y):
            ret = []
            for iy, ix in _it.product(range(y), range(x)):
                idx = iy * (x + 1 if is_y else x) + ix
                if idx >= len(d) or idx + (1 if is_y else x) >= len(d):
                    print('get')
                ret.append((d[idx] + d[idx + (1 if is_y else x)]) / 2)
            return ret

        dim, x, y = LoaderMd2d.read_dim(file)

        with open(file) as f:
            active = False
            data = []
            buf = []
            for s in f:
                if active:
                    if LoaderMd2d.pat_split.match(s):
                        buf = convert_(s) + buf
                    else:
                        active = False
                        data.append(buf)
                        buf = []
                else:
                    if LoaderMd2d.pat_data.match(s):
                        active = True
                        buf += convert_(s)

        if dim == 2:
            data = [average_(j, i % 2) for i, j in enumerate(data)]

        return data


manager = Manager()


def init(configs):
    return manager.init(configs)


def args(name):
    return manager.args(name)


def load(name, arguments):
    return manager.load(name, arguments)


# if __name__ == '__main__':
#     init([])
#     d = 'D:\Work\FYP\software\data'
#     i = 'D:\Work\FYP\software\input\md2d\pdp_demo.md2d'
#     fs = [_os.path.join(d, i) for i in _os.listdir(d)]
#     d, r = load('MD2D', [fs, [i]])
#     d, r = d[0]['children'][0]['quantities'][3]['data']()

# linux
# if __name__ == "__main__":
# d = '/run/media/towdium/Files/Work/FYP/software/data'
# files_ = [_os.path.join(d, i) for i in _os.listdir(d)]
# a = args('MD2D')
# d, r = load('MD2D', [files_, '/run/media/towdium/Files/Work/FYP/software/input/md2d/hcd_demo.md2d'])
# d, r = d[0]['children'][0]['quantities'][3]['data']()  #pylint: disable=E1126
