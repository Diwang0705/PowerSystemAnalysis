from model import OneLineModel
from parser import parse_bus, parse_branch

def import_psse_one_line(path):
    model = OneLineModel()
    section = None

    with open(path, 'r') as f:
        for line in f:
            line = line.strip()

            # Skip empty lines or comments
            if not line or line.startswith('!'):
                continue

            # -------------------------
            # Detect section endings
            # -------------------------
            if "END OF BUS DATA" in line:
                section = None
                continue
            if "END OF LOAD DATA" in line:
                section = None
                continue
            if "END OF GENERATOR DATA" in line:
                section = None
                continue
            if "END OF BRANCH DATA" in line:
                section = None
                continue
            if "END OF TRANSFORMER DATA" in line:
                section = None
                continue

            # -------------------------
            # Detect section starts
            # -------------------------
            if "BEGIN LOAD DATA" in line:
                section = "LOAD"
                continue
            if "BEGIN GENERATOR DATA" in line:
                section = "GEN"
                continue
            if "BEGIN BRANCH DATA" in line:
                section = "BRANCH"
                continue
            if "BEGIN TRANSFORMER DATA" in line:
                section = "TX"
                continue

            # BUS DATA has no explicit "BEGIN" marker in your file
            # It starts immediately after the header lines
            if section is None and line[0].isdigit() and "'" in line:
                # This is BUS DATA
                section = "BUS"

            # -------------------------
            # Parse BUS
            # -------------------------
            if section == "BUS":
                # Stop if we hit the END marker
                if "END OF BUS DATA" in line:
                    section = None
                    continue

                # Parse bus line
                bus = parse_bus(line)
                if bus:
                    model.buses[bus.bus_id] = bus
                continue

            # -------------------------
            # Parse BRANCH
            # -------------------------
            if section == "BRANCH":
                if "END OF BRANCH DATA" in line:
                    section = None
                    continue

                branch = parse_branch(line)
                model.branches.append(branch)
                continue

            # -------------------------
            # Ignore LOAD, GEN, TX for now
            # -------------------------
            if section in ("LOAD", "GEN", "TX"):
                continue

    return model
