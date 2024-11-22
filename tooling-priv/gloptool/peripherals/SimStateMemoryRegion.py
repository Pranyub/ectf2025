import angr

class SimStateMemoryRegion(angr.SimStatePlugin):
    def __init__(self, regions):
        super(SimStateMemoryRegion, self).__init__()
        self.regions = regions
    
    @angr.SimStatePlugin.memo
    def copy(self, memo):
        return SimStateMemoryRegion(self.regions)