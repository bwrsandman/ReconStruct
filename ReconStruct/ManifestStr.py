try:
    from ReconStruct.ManifestBase import ManifestBase
except ImportError:
    from ManifestBase import ManifestBase


class ManifestStr(ManifestBase):
    def __init__(self, label, size, parent=None):
        super(ManifestStr, self).__init__(label, size, parent)

    def __call__(self, data, start=0):
        return data[start:start + self.size].decode("utf-8"), self.size
