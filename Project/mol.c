#include "mol.h"

/*This function should copy the values pointed to by element, x, y, and z into the atom stored at
atom. You may assume that sufficient memory has been allocated at all pointer addresses.
Note that using pointers for the function “inputs”, x, y, and z, is done here to match the
function arguments of atomget.*/
void atomset(atom *atom, char element[3], double *x, double *y, double *z)
{
	strcpy(atom->element, element); // Copies atom values
	atom->x = *x;					// Copies atom values
	atom->y = *y;					// Copies atom values
	atom->z = *z;					// Copies atom values
}

/*This function should copy the values in the atom stored at atom to the locations pointed to by
element, x, y, and z. You may assume that sufficient memory has been allocated at all pointer
addresses. Note that using pointers for the function “input”, atom, is done here to match the
function arguments of atomset.*/
void atomget(atom *atom, char element[3], double *x, double *y, double *z)
{
	strcpy(element, atom->element); // Copies atom values
	*x = atom->x;					// Copies atom values
	*y = atom->y;					// Copies atom values
	*z = atom->z;					// Copies atom values
}

/*This function should copy the values a1, a2 and epairs into the corresponding structure
attributes in bond. You may assume that sufficient memory has been allocated at all pointer
addresses. Note you are not copying atom structures, only the addresses of the atom
structures.*/
void bondset(bond *bond, unsigned short *a1, unsigned short *a2, atom **atoms, unsigned char *epairs)
{
	bond->a1 = *a1;		   // Copies bond values
	bond->a2 = *a2;		   // Copies bond values
	bond->atoms = *atoms;   // Copies bond values
	bond->epairs = *epairs; // Copies bond values
	compute_coords(bond);
}

/*This function should copy the structure attributes in bond to their corresponding arguments:
a1, a2 and epairs. You may assume that sufficient memory has been allocated at all pointer
addresses. Note you are not copying atom structures, only the addresses of the atom
structures.*/
void bondget(bond *bond, unsigned short *a1, unsigned short *a2, atom **atoms, unsigned char *epairs)
{
	*a1 = bond->a1;			// Copies bond values
	*a2 = bond->a2;			// Copies bond values
	*epairs = bond->epairs; // Copies bond values
	*atoms = bond->atoms;	// Copies bond values
}

void compute_coords(bond *bond)
{
	atom *temp1 = &bond->atoms[bond->a1];
	atom *temp2 = &bond->atoms[bond->a2];

	bond->x1 = temp1->x;
	bond->x2 = temp2->x;

	bond->y1 = temp1->y;
	bond->y2 = temp2->y;

	bond->z = (temp1->z + temp2->z) / 2;

	bond->len = sqrt(pow(temp2->x - temp1->x, 2) + pow(temp2->y - temp1->y, 2));

	bond->dx = (temp2->x - temp1->x) / bond->len;
	bond->dy = (temp2->y - temp1->y) / bond->len;
}

/*This function should return the address of a malloced area of memory, large enough to hold a
molecule. The value of atom_max should be copied into the structure; the value of atom_no in
the structure should be set to zero; and, the arrays atoms and atom_ptrs should be malloced
to have enough memory to hold atom_max atoms and pointers (respectively). The value of
bond_max should be copied into the structure; the value of bond_no in the structure should be
set to zero; and, the arrays bonds and bond_ptrs should be malloced to have enough memory
to hold bond_max bonds and pointers (respectively).*/
molecule *molmalloc(unsigned short atom_max, unsigned short bond_max)
{
	molecule *molDiff = (molecule *)malloc(sizeof(molecule)); // Mallocs size of molecule and casts it

	molDiff->atom_max = atom_max;							// Copies values into struct
	molDiff->atom_no = 0;									// Sets value to 0
	molDiff->atoms = malloc(atom_max * sizeof(atom));		// Malloced to have enough memory for max atoms
	molDiff->atom_ptrs = malloc(atom_max * sizeof(atom *)); // Malloced to have enough memory for max pointers

	molDiff->bond_max = bond_max;							// Copies values into struct
	molDiff->bond_no = 0;									// Sets value to 0
	molDiff->bonds = malloc(bond_max * sizeof(bond));		// Malloced to have enough memory for max bonds
	molDiff->bond_ptrs = malloc(bond_max * sizeof(bond *)); // Malloced to have enough memory for max pointers

	return molDiff; // returns the malloced struct
}

/*This function should return the address of a malloced area of memory, large enough to hold a
molecule. Additionally, the values of atom_max, atom_no, bond_max, bond_no should be
copied from src into the new structure. Finally, the The arrays atoms, atom_ptrs, bonds and
bond_ptrs must be allocated to match the size of the ones in src. Finally, you should use
molappend_atom and molappend_bond (below) to add the atoms from the src to the new
molecule (note that this will also initialize the corresponding pointer arrays). You should re-use
(i.e. call) the molmalloc function in this function.*/
molecule *molcopy(molecule *src)
{
	molecule *molDiff = molmalloc(src->atom_max, src->bond_max); // Mallocs size of molecule and casts it

	molDiff->atom_max = src->atom_max; // Copies values into struct
	molDiff->bond_max = src->bond_max; // Copies values into struct

	for (int i = 0; i < src->atom_no; i++) // Loops to append all the molecules
	{
		molappend_atom(molDiff, &src->atoms[i]);
	}
	for (int i = 0; i < src->bond_no; i++) // Loops to append all the bonds
	{
		molappend_bond(molDiff, &src->bonds[i]);
	}

	return molDiff; // Return address
}

/*This function should free the memory associated with the molecule pointed to by ptr.
This includes the arrays atoms, atom_ptrs, bonds, bond_ptrs.*/
void molfree(molecule *ptr)
{
	free(ptr->atoms);	  // Frees mem
	free(ptr->atom_ptrs); // Frees mem
	free(ptr->bonds);	  // Frees mem
	free(ptr->bond_ptrs); // Frees mem
	free(ptr);			  // Frees the pointer itself
}

/*This function should copy the data pointed to by atom to the first “empty” atom in atoms in the
molecule pointed to by molecule, and set the first “empty” pointer in atom_ptrs to the same
atom in the atoms array incrementing the value of atom_no. If atom_no equals atom_max, then
atom_max must be incremented, and the capacity of the atoms, and atom_ptrs arrays
increased accordingly. If atom_max was 0, it should be incremented to 1, otherwise it should be
doubled. Increasing the capacity of atoms should be done using realloc so
that a larger amount of memory is allocated and the existing data is copied to the new location.
IMPORTANT: After mallocing or reallocing enough memory for atom_ptrs, these pointers
should be made to point to the corresponding atoms in the new atoms array (not the old array
which may have been freed)*/
void molappend_atom(molecule *molecule, atom *atom)
{
	if (molecule->atom_max == 0)
	{
		molecule->atom_max = 1; // If empty than increase to 1
	}
	else if (molecule->atom_max <= molecule->atom_no)
	{
		molecule->atom_max *= 2; // Doubles capacity of atoms
	}

	molecule->atoms = realloc(molecule->atoms, molecule->atom_max * sizeof(*atom));			// reallocate more mem based on max atoms
	molecule->atom_ptrs = realloc(molecule->atom_ptrs, molecule->atom_max * sizeof(*atom)); // reallocate more mem based on max atoms

	if (molecule->atoms == NULL) // Error check
	{
		printf("Mermory reallocation error for molecule->atoms");
		return;
	}
	if (molecule->atom_ptrs == NULL) // Error check
	{
		printf("Mermory reallocation error for molecule->atoms_ptrs");
		return;
	}

	for (int i = 0; i < molecule->atom_no; i++)
	{
		molecule->atom_ptrs[i] = &molecule->atoms[i]; // Point atom_pts to new atoms
	}

	atomset(molecule->atoms + molecule->atom_no, atom->element, &atom->x, &atom->y, &atom->z); // Copy data
	molecule->atom_ptrs[molecule->atom_no] = &(molecule->atoms[molecule->atom_no]);			   // Set atom pointers to point to atoms
	molecule->atom_no++;																	   // Go to next atom
}

/*This function should operate like that molappend_atom function, except for bonds*/
void molappend_bond(molecule *molecule, bond *bond)
{
	if (molecule->bond_max == 0)
	{
		molecule->bond_max = 1;
	}
	else if (molecule->bond_max <= molecule->bond_no)
	{
		molecule->bond_max *= 2;
	}

	molecule->bonds = realloc(molecule->bonds, molecule->bond_max * sizeof(*bond));
	molecule->bond_ptrs = realloc(molecule->bond_ptrs, molecule->bond_max * sizeof(*bond));

	if (molecule->bonds == NULL)
	{
		printf("Mormory reallocation error");
		return;
	}
	if (molecule->bond_ptrs == NULL)
	{
		printf("Mormory reallocation error");
		return;
	}

	for (int i = 0; i < molecule->bond_no; i++)
	{
		molecule->bond_ptrs[i] = &molecule->bonds[i];
	}

	bondset(molecule->bonds + molecule->bond_no, &bond->a1, &bond->a2, &bond->atoms, &bond->epairs);
	molecule->bond_ptrs[molecule->bond_no] = &(molecule->bonds[molecule->bond_no]);
	molecule->bond_no++;
}
/*Compares the values of a and b atoms and returns a signed int*/
int compareAtoms(const void *a, const void *b)
{
	return (*(double *)a - *(double *)b);
}

/*Compares the values of a and b bonds and returns a signed int*/
int compareBonds(const void *a, const void *b)
{
	bond **bond1 = (bond **)a;
	bond **bond2 = (bond **)b;

	return ((*bond1)->z - (*bond2)->z);
}

/*This function should sort the atom_ptrs array in place in order of increasing z value. I.e.
atom_ptrs[0] should point to the atom that contains the lowest z value and
atom_ptrs[atom_no-1] should contain the highest z value. It should also sort the bond_ptrs
array in place in order of increasing “ z value”. Since bonds don’t have a z attribute, their z
value is assumed to be the average z value of their two atoms. I.e. bond_ptrs[0] should point
to the bond that has the lowest z value and bond_ptrs[atom_no-1] should contain the highest
z value.Hint: use qsort.*/
void molsort(molecule *molecule)
{
	qsort(molecule->atom_ptrs, molecule->atom_no, sizeof(atom *), compareAtoms);
	qsort(molecule->bond_ptrs, molecule->bond_no, sizeof(bond *), compareBonds);
}

/*This function will set the values in an affine transformation
matrix, xform_matrix, corresponding to a rotation of deg degrees around the x-axis.*/
void xrotation(xform_matrix xform_matrix, unsigned short deg)
{
	/*Basic rotation math using equations found on google*/
	xform_matrix[0][0] = 1;
	xform_matrix[1][0] = 0;
	xform_matrix[2][0] = 0;
	xform_matrix[0][1] = 0;
	xform_matrix[1][1] = cos((deg) * (M_PI / 180));
	xform_matrix[2][1] = sin((deg) * (M_PI / 180));
	xform_matrix[0][2] = 0;
	xform_matrix[1][2] = -sin((deg) * (M_PI / 180));
	xform_matrix[2][2] = cos((deg) * (M_PI / 180));
}

/*This function will set the values in an affine transformation
matrix, xform_matrix, corresponding to a rotation of deg degrees around the y-axis.*/
void yrotation(xform_matrix xform_matrix, unsigned short deg)
{
	/*Basic rotation math using equations found on google*/
	xform_matrix[0][0] = cos((deg) * (M_PI / 180));
	xform_matrix[1][0] = 0;
	xform_matrix[2][0] = -sin((deg) * (M_PI / 180));
	xform_matrix[0][1] = 0;
	xform_matrix[1][1] = 1;
	xform_matrix[2][1] = 0;
	xform_matrix[0][2] = sin((deg) * (M_PI / 180));
	xform_matrix[1][2] = 0;
	xform_matrix[2][2] = cos((deg) * (M_PI / 180));
}

/*This function will set the values in an affine transformation
matrix, xform_matrix, corresponding to a rotation of deg degrees around the z-axis.*/
void zrotation(xform_matrix xform_matrix, unsigned short deg)
{
	/*Basic rotation math using equations found on google*/
	xform_matrix[0][0] = cos((deg) * (M_PI / 180));
	xform_matrix[1][0] = sin((deg) * (M_PI / 180));
	xform_matrix[2][0] = 0;
	xform_matrix[0][1] = -sin((deg) * (M_PI / 180));
	xform_matrix[1][1] = cos((deg) * (M_PI / 180));
	xform_matrix[2][1] = 0;
	xform_matrix[0][2] = 0;
	xform_matrix[1][2] = 0;
	xform_matrix[2][2] = 1;
}

/*This function will apply the transformation matrix to all the atoms of the molecule by
performing a vector matrix multiplication on the x, y, z coordinates.*/
void mol_xform(molecule *molecule, xform_matrix matrix)
{
	double x, y, z;
	for (int i = 0; i < molecule->atom_no; i++)
	{
		x = molecule->atoms[i].x;
		y = molecule->atoms[i].y;
		z = molecule->atoms[i].z;
		molecule->atoms[i].x = (x * matrix[0][0]) + (y * matrix[0][1]) + (z * matrix[0][2]);
		molecule->atoms[i].y = (x * matrix[1][0]) + (y * matrix[1][1]) + (z * matrix[1][2]);
		molecule->atoms[i].z = (x * matrix[2][0]) + (y * matrix[2][1]) + (z * matrix[2][2]);
	}
	for (int i = 0; i < molecule->bond_no; i++)
	{
		compute_coords(&molecule->bonds[i]);
	}
}
