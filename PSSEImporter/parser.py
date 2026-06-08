from model import Bus, Branch

def detect_section(line):
    if "END OF BUS DATA" in line:
        return "END_BUS"
    if "END OF LOAD DATA" in line:
        return "END_LOAD"
    if "END OF GENERATOR DATA" in line:
        return "END_GEN"
    if "END OF BRANCH DATA" in line:
        return "END_BRANCH"
    if "END OF TRANSFORMER DATA" in line:
        return "END_TX"

    # Section starts (implicit)
    if line.strip().startswith("1,'") or line.strip().startswith("2,'") or line.strip().startswith("3,'"):
        return "BUS_START"

    return None

def parse_bus(line):
    # Split and clean
    parts = [p.strip() for p in line.split(',') if p.strip()]
    try:
        bus_id = int(parts[0])
        name = parts[1].strip("'").strip()
        base_kv = float(parts[2])
        # Return a formatted name with voltage
        return Bus(bus_id, f"{name} ({base_kv:.3f} kV)")
    except (ValueError, IndexError):
        return None

def parse_branch(line):
    parts = [p.strip() for p in line.split(',')]
    from_bus = int(parts[0])
    to_bus = int(parts[1])
    return Branch(from_bus, to_bus)