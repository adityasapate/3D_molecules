from rdkit import Chem
from rdkit.Chem import AllChem
import sys



x = Chem.MolFromSmiles('CC(=O)NCCC1=CNc2c1cc(OC)cc2')# paste the smiles code for the chemical formula here

x = Chem.AddHs(x)
AllChem.Compute2DCoords(x)
AllChem.EmbedMolecule(x)
AllChem.UFFOptimizeMolecule(x)
print Chem.MolToMolBlock(x)

