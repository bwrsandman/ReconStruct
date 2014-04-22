class ManifestBase(object):
    def __init__(self, label, size, parent=None):
        self.label = label
        self._size = size
        self.parent = parent

    @property
    def size(self):
        ret = 0
        if type(self._size) is int:
            ret = self._size
        else:
            ret = self.parent.actual_size_of(self._size)
        assert type(ret) is int
        return ret
