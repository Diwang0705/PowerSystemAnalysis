from importer import import_psse_one_line

model = import_psse_one_line("ProjectELEC1.raw")

print("Buses:")
for bus in model.buses.values():
    print(f"  {bus.bus_id}: {bus.name}")

print("\nBranches:")
for br in model.branches:
    print(f"  {br.from_bus} -> {br.to_bus}")