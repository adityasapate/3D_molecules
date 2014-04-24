python converter.py > ./data/temp.mol
babel -imol ./data/temp.mol -ocml -O ./output.cml
g++ main.cpp -lGL -lGLU -lglut -o ./3Dmodel
./3Dmodel ./output.cml
