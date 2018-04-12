make planar_discrete_driver
./planar_discrete_driver --height 100 --width 100 --rscale 10 --cscale 10  --sizemin 0 --sizemax 10 --count 1000 > ./json/small.json

./planar_discrete_driver --height 10000 --width 10000 --rscale 10 --cscale 10  --sizemin 0 --sizemax 10 --count 1000 > ./json/smalltall.json
