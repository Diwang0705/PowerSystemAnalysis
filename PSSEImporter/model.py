class Bus:
    def __init__(self, bus_id, name):
        self.bus_id = bus_id
        self.name = name

class Branch:
    def __init__(self, from_bus, to_bus):
        self.from_bus = from_bus
        self.to_bus = to_bus

class OneLineModel:
    def __init__(self):
        self.buses = {}
        self.branches = []