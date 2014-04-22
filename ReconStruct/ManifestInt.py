try:
    from ReconStruct.ManifestBase import ManifestBase
except ImportError:
    from ManifestBase import ManifestBase


class ManifestInt(ManifestBase):
    def __init__(self, label, size, parent=None):
        super(ManifestInt, self).__init__(label, size, parent)

    def __call__(self, data, start=0, byteorder='big'):
        sub_data = data[start:start + self.size]
        try:
            return int.from_bytes(sub_data, byteorder), self.size
        except AttributeError:
            return int(sub_data.encode('hex'), 16), self.size
