echo "sequencial"
./laplace_seq_iteracoes 1000
echo "pthread"
./laplace_pth 1000 1
./laplace_pth 1000 2
./laplace_pth 1000 4
./laplace_pth 1000 8
echo "comparing results, if it's blank than they are equal:"
diff grid_laplace.txt grid_laplace_8threads.txt