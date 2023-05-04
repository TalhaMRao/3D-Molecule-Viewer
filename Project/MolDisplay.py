import molecule

header = """<svg version="1.1" width="1000" height="1000" xmlns="http://www.w3.org/2000/svg">"""

footer = """</svg>"""

offsetx = 500
offsety = 500

##############################################################################################################
# Atom class
class Atom:
    def __init__(self, c_atom): # constructor for Atom
        self.c_atom = c_atom 
        self.z = c_atom.z

    def svg(self):
        cx = offsetx + ((self.c_atom.x) * 100.0)
        cy = offsety + ((self.c_atom.y) * 100.0)
        r = radius[self.c_atom.element]
        fill = element_name[self.c_atom.element]

        return '  <circle cx="%.2f" cy="%.2f" r="%d" fill="url(#%s)"/>\n' % (cx, cy, r, fill)

##############################################################################################################
# Bond class
class Bond:
    def __init__(self, c_bond): # contructor for Bond
        self.c_bond = c_bond
        self.z = c_bond.z

    def svg(self):
        dx = self.c_bond.dx
        dy = self.c_bond.dy

        x1 = offsetx - (dy * 10) + (self.c_bond.x1 * 100.0)
        x2 = offsetx + (dy * 10) + (self.c_bond.x1 * 100.0)
        x3 = offsetx + (dy * 10) + (self.c_bond.x2 * 100.0)
        x4 = offsetx - (dy * 10) + (self.c_bond.x2 * 100.0)

        y1 = offsety + (dx * 10) + (self.c_bond.y1 * 100.0)
        y2 = offsety - (dx * 10) + (self.c_bond.y1 * 100.0)
        y3 = offsetx - (dx * 10) + (self.c_bond.y2 * 100.0)
        y4 = offsetx + (dx * 10) + (self.c_bond.y2 * 100.0)

        return '  <polygon points="%.2f,%.2f %.2f,%.2f %.2f,%.2f %.2f,%.2f" fill="green"/>\n' % (x1, y1, x2, y2, x3, y3, x4, y4)

##############################################################################################################
# Molecule sub class
class Molecule(molecule.molecule):
    def svg(self):
        final = header
        bonds = []
        atoms = []
        molecules = []
        
        for i in range(self.bond_no):
            bonds.append(Bond(self.get_bond(i)))

        for i in range(self.atom_no):
            atoms.append(Atom(self.get_atom(i)))

        molecules = atoms + bonds
        molecules.sort(key=lambda sorted: sorted.z) # weird function, sorts on z values

        for i in range(len(molecules)):
            final += molecules[i].svg() # add sorted molecules
        final += footer

        return final

    def parse(self, file):
        lines = file.readlines() # read lines

        values = lines[3].split() # splits based on spaces between chars

        num_of_atoms = int(values[0]) # convert chars to ints
        num_of_bonds = int(values[1]) # chars to ints

        for i in range(num_of_atoms):
            temp = lines[i + 4].split()
            self.append_atom(str(temp[3]), float(temp[0]), float(temp[1]), float(temp[2])) 

        for i in range(num_of_bonds):
            temp = lines[i + 4 + num_of_atoms].split()
            self.append_bond(int(temp[0])-1, int(temp[1])-1, int(temp[2]))


#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. 
