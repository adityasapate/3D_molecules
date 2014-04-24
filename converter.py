from rdkit import Chem
from rdkit.Chem import AllChem
import sys



x = Chem.MolFromSmiles('C#C')

x = Chem.AddHs(x)
AllChem.Compute2DCoords(x)
AllChem.EmbedMolecule(x)
AllChem.UFFOptimizeMolecule(x)
print Chem.MolToMolBlock(x)

