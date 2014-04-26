try:
    from ReconStruct.ManifestBase import ManifestBase
except ImportError:
    from ManifestBase import ManifestBase


class ManifestCustom(ManifestBase):
    def __init__(self, label, size, parent=None):
        super(ManifestCustom, self).__init__(label, size, parent)
        self.submanifests = []

    def add(self, manifest):
        self.submanifests.append(manifest)
        manifest.parent = self

    def actual_size_of(self, label):
        if label in self._current_data:
            ret = self._current_data[label]
        elif self.parent:
            ret = self.parent.actual_size_of(label)
        else:
            ret = 0
        assert type(ret) is int
        return ret

    def __call__(self, data, start=0):
        ret = []
        parsed = -start
        for i in range(self.size):
            self._current_data = dict()
            sub_ret = []
            for manifest in self.submanifests:
                formatted, shift = manifest(data, start)
                sub_ret.append(formatted)
                start += shift
                self._current_data[manifest.label] = formatted
            ret.append(sub_ret)
        del self._current_data
        return ret, parsed + start
