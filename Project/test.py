import molecule
import MolDisplay

file = open("water-3D.sdf", "r")

water_molecule = MolDisplay.Molecule()

water_molecule.parse(file)


tempAtom = MolDisplay.Atom(water_molecule.get_atom(0))
tempAtom1 = MolDisplay.Atom(water_molecule.get_atom(1))
tempAtom2 = MolDisplay.Atom(water_molecule.get_atom(2))

print(tempAtom)
print(tempAtom1)
print(tempAtom2)