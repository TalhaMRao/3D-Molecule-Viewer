import molecule
import sqlite3
import os
import MolDisplay

class Database:
    def __init__(self, reset=False):
        if reset and os.path.exists('molecules.db'):
            os.remove('molecules.db')
        self.conn = sqlite3.connect('molecules.db')
    
    def create_tables(self):
        self.conn.execute("""CREATE TABLE if not exists Elements (
                            ELEMENT_NO      INTEGER         NOT NULL,
                            ELEMENT_CODE    VARCHAR(3)      NOT NULL,
                            ELEMENT_NAME    VARCHAR(32)     NOT NULL,
                            COLOUR1         CHAR(6)         NOT NULL,
                            COLOUR2         CHAR(6)         NOT NULL,
                            COLOUR3         CHAR(6)         NOT NULL,
                            RADIUS          DECIMAL(3)      NOT NULL,
                            PRIMARY KEY (ELEMENT_CODE));""")
        
        self.conn.execute("""CREATE TABLE if not exists Atoms (
                            ATOM_ID         INTEGER         PRIMARY KEY     AUTOINCREMENT      NOT NULL,
                            ELEMENT_CODE    VARCHAR(3)      NOT NULL,
                            X               DECIMAL(7,4)    NOT NULL,
                            Y               DECIMAL(7,4)    NOT NULL,
                            Z               DECIMAL(7,4)    NOT NULL, 
                            FOREIGN KEY (ELEMENT_CODE) REFERENCES Elements (ELEMENT_CODE));""")
        
        self.conn.execute("""CREATE TABLE if not exists Bonds (
                            BOND_ID         INTEGER         PRIMARY KEY AUTOINCREMENT      NOT NULL,
                            A1              INTEGER         NOT NULL,
                            A2              INTEGER         NOT NULL,
                            EPAIRS          INTEGER         NOT NULL);""")
        
        self.conn.execute("""CREATE TABLE if not exists Molecules (
                            MOLECULE_ID     INTEGER         PRIMARY KEY AUTOINCREMENT      NOT NULL,
                            NAME            TEXT            NOT NULL    UNIQUE);""")

        self.conn.execute("""CREATE TABLE if not exists MoleculeAtom (
                            MOLECULE_ID     INTEGER     NOT NULL,
                            ATOM_ID         INTEGER     NOT NULL,
                            PRIMARY KEY (MOLECULE_ID, ATOM_ID),
                            FOREIGN KEY (MOLECULE_ID) REFERENCES Molecules (MOLECULE_ID),
                            FOREIGN KEY (ATOM_ID) REFERENCES Atoms (ATOM_ID));""")
        
        self.conn.execute("""CREATE TABLE if not exists MoleculeBond (
                            MOLECULE_ID     INTEGER     NOT NULL,
                            BOND_ID         INTEGER     NOT NULL,
                            PRIMARY KEY (MOLECULE_ID, BOND_ID),
                            FOREIGN KEY (MOLECULE_ID)   REFERENCES Molecules (MOLECULE_ID),
                            FOREIGN KEY (BOND_ID)       REFERENCES Bonds (BOND_ID));""")

    def __setitem__(self, table, values):
        self.conn.execute("""INSERT INTO {} VALUES {} 
                        """.format(table, values))

    def add_atom(self, molname, atom):
        self.conn.execute("""INSERT INTO Atoms (ELEMENT_CODE, X, Y, Z) 
                            VALUES (?, ?, ?, ?)""", (atom.element, atom.x, atom.y, atom.z,))
        molecule = self.conn.execute("""SELECT MOLECULE_ID FROM Molecules 
                                    WHERE NAME = ?""", (molname,)).fetchone()[0]
        atom = self.conn.execute("""SELECT MAX(ATOM_ID) FROM Atoms
                                    WHERE ELEMENT_CODE = ?""", (atom.element,)).fetchone()[0]
        self.conn.execute("""INSERT INTO MoleculeAtom (ATOM_ID, MOLECULE_ID) VALUES ({}, {})""" .format(atom, molecule))
        self.conn.commit()

    def add_bond(self, molname, bond):
        self.conn.execute("""INSERT INTO Bonds (A1, A2, EPAIRS) 
                            VALUES (?, ?, ?)""", (bond.a1, bond.a2, bond.epairs,))
        molecule = self.conn.execute("""SELECT MOLECULE_ID FROM Molecules 
                                        WHERE NAME = ?""", (molname,)).fetchone()[0]
        bond = self.conn.execute("""SELECT BOND_ID FROM Bonds 
                                        WHERE A1 = ? AND A2 = ? AND EPAIRS = ?""", (bond.a1, bond.a2, bond.epairs,)).fetchone()[0]
        self.conn.execute("""INSERT INTO MoleculeBond (MOLECULE_ID, BOND_ID) VALUES ({}, {})""".format(molecule, bond))
        self.conn.commit()

    def add_molecule(self, name, fp):
        addMolecule1Test = MolDisplay.Molecule()
        addMolecule1Test.parse(fp)

        self.conn.execute("""INSERT INTO Molecules (NAME) 
                            VALUES (?)""", (name,))
        
        for i in range(addMolecule1Test.bond_no):
            self.add_bond(name, addMolecule1Test.get_bond(i))
        for i in range(addMolecule1Test.atom_no):
            self.add_atom(name, addMolecule1Test.get_atom(i))
        self.conn.commit()
    
    def load_mol(self, name):
        addMolecule1Test = MolDisplay.Molecule()
        atoms = self.conn.execute("""SELECT Atoms.* FROM Molecules JOIN MoleculeAtom ON Molecules.MOLECULE_ID = MoleculeAtom.MOLECULE_ID
                JOIN Atoms ON MoleculeAtom.ATOM_ID = Atoms.ATOM_ID WHERE Molecules.Name = ? ORDER BY Atoms.ATOM_ID ASC""", (name,)).fetchall()
        bonds = self.conn.execute("""SELECT Bonds.* FROM Molecules JOIN MoleculeBond ON Molecules.MOLECULE_ID = MoleculeBond.MOLECULE_ID
                JOIN Bonds ON MoleculeBond.BOND_ID = Bonds.BOND_ID WHERE Molecules.Name = ? ORDER BY Bonds.BOND_ID ASC""", (name,)).fetchall()
        for i in atoms:
            addMolecule1Test.append_atom(i[1], i[2], i[3], i[4])
        for i in bonds:
            addMolecule1Test.append_bond(i[1], i[2], i[3])
        self.conn.commit()
        return addMolecule1Test
    
    def radius(self):
        radius = {}
        for i in self.conn.execute("""SELECT ELEMENT_CODE, RADIUS FROM ELEMENTS""").fetchall():
            radius[i[0]] = i[1]
        return radius
    
    def element_name(self):
        element_name = {}
        for i in self.conn.execute("""SELECT ELEMENT_CODE, ELEMENT_NAME FROM Elements""").fetchall():
            element_name[i[0]] = i[1]
        return element_name

    def radial_gradients(self):

        radialPackage = self.conn.execute("""SELECT ELEMENT_NAME, COLOUR1, COLOUR2, COLOUR3 FROM Elements""").fetchall()
        radialGradientSVG = """
                <radialGradient id="%s" cx="-50%%" cy="-50%%" r="220%%" fx="20%%" fy="20%%">
                 <stop offset="0%%" stop-color="#%s"/>
                 <stop offset="50%%" stop-color="#%s"/>
                 <stop offset="100%%" stop-color="#%s"/>
                </radialGradient>"""
        
        concatatanations = []
        for i in radialPackage:
            concatatanations += (radialGradientSVG % (i[0], i[1], i[2], i[3]))
        empty = ""
        return empty.join(concatatanations)

if __name__ == "__main__":
    db = Database(reset=True)
    db.create_tables()

    db['Elements'] = (1 , 'H', 'Hydrogen', 'FFFFFF', '050505', '020202', 25)
    db['Elements'] = (6 , 'C', 'Carbon', '808080', '010101', '000000', 40)
    db['Elements'] = (7 , 'N', 'Nitrogen', '0000FF', '000005', '000002', 40)
    db['Elements'] = (8 , 'O', 'Oxygen', 'FF0000', '050000', '020000', 40)

    fp = open( 'water-3D-structure-CT1000292221.sdf')
    db.add_molecule( 'Water', fp )
    fp = open( 'caffeine-3D-structure-CT1001987571.sdf' );
    db.add_molecule( 'Caffeine', fp );
    fp = open( 'CID_31260.sdf' );
    db.add_molecule( 'Isopentanol', fp );
    #display tables

    print( db.conn.execute( "SELECT * FROM Elements;" ).fetchall() );
    print( db.conn.execute( "SELECT * FROM Molecules;" ).fetchall() );
    print( db.conn.execute( "SELECT * FROM Atoms;" ).fetchall() );
    print( db.conn.execute( "SELECT * FROM Bonds;" ).fetchall() );
    print( db.conn.execute( "SELECT * FROM MoleculeAtom;" ).fetchall() );
    print( db.conn.execute( "SELECT * FROM MoleculeBond;" ).fetchall() );

    #######################################################################################################

    db = Database(reset=False)

    MolDisplay.radius = db.radius()
    MolDisplay.element_name = db.element_name()
    MolDisplay.header += db.radial_gradients()

    for molecule in ['Water', 'Caffeine', 'Isopentanol']:
        mol = db.load_mol(molecule)
        mol.sort()
        fp = open(molecule + ".svg", "w")
        fp.write(mol.svg())
        fp.close()
