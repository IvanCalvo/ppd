echo "sequencial"
./laplace_seq_iteracoes 1000
echo "pthread"
./laplace_pth_test 1000 1
./laplace_pth_test 1000 2
./laplace_pth_test 1000 4
./laplace_pth_test 1000 8