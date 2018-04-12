make planar_discrete_driver
./planar_discrete_driver --xmax 100 --ymax 100 --xscale 10 --yscale 10  --sizemin 0 --sizemax 10 --count 1000 > ./json/small.json

./planar_discrete_driver --xmax 10000 --ymax 10000 --xscale 10 --yscale 10  --sizemin 0 --sizemax 10 --count 1000 > ./json/smalltall.json
