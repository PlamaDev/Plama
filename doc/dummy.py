class Loader:
    @staticmethod
    def name():
        return 'Dummy'  # plugin name

    @staticmethod
    def load():
        return [{
            'abbr': 'Rea-1',  # abbreviation of node name
            'name': 'Reaction A + B <=> C',  # full node name
            'children': [],  # child nodes
            'quantities': [{  # quantities in this node
                'name': 'example-2D1D',  # name of quantity
                'times': [1, 2, 3],  # times of quantity
                'dimData': 1,  # dimension amout of quantity
                'sizeData': [3, 3],  # size for one set of data
                'sizeModel': [[0, 1], [0, 1]],  # size of actual model
                'labels': ['t(s)', '&Phi;(Wm<sup>-2</sup>)', 'x(m)', 'y(m)'],  # axis
                'data': lambda: [  # data should be a function for lazy loading
                    [0, 1, 0, 0, 0, 0, 0, 0, 0],  # 3x3 data at time 1
                    [0, 0, 1, 0, 0, 0, 0, 0, 0],  # 3x3 data at time 2
                    [0, 0, 0, 0, 2, 0, 0, 0, 0]   # 3x3 data at time 3
                ]
            }, {
                'name': 'example-0D1D',  # functional data example
                'times': [1, 2, 3],
                'dimData': 1,
                'sizeData': [],
                'sizeModel': [],
                'labels': ['t(s)', 'h(m)'],
                'data': lambda: [
                    [0], [3], [2]
                ]
            }, {
                'name': 'example-2D2D',  # 2-dimensional data example (usually fields)
                'times': [1],
                'dimData': 2,  # dimension = 2
                'sizeData': [3, 2],
                'sizeModel': [[2, 20], [4, 5]],
                'labels': ['t(s)', 'h(m)', 'x(m)', 'y(m)'],
                'data': lambda: [
                    [0, 1, 0, 0, 0, 0],  # 3x2 data at time 1, dimension 1
                    [0, 0, 1, 0, 0, 0]   # 3x2 data at time 1, dimension 2
                ]
            }, {
                'name': 'example-err',
                'times': [1, 2, 3],
                'dimData': 1,
                'sizeData': [],
                'sizeModel': [],
                'labels': ['t(s)', 'h(m)'],
                'data': lambda: [[1], [2]]
            }]
        }]

    @staticmethod
    def args():
        return []  # 

plugin = Loader()
