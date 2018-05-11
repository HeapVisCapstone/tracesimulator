To install boost statically linked libraries:
https://stackoverflow.com/a/24086375

## Getting the Data
For each dataset, you will need a .hierarchy, .trace and .names file. 

See [here](https://github.com/HeapVisCapstone/documents) for more information.

## Scatterplot of Embedding
To run the planar embedding visualization

```
make embed_trace
./embed_trace traces/[file].names json/[write to file].json [--trimdegree=n] < traces/[file].trace

python -m SimpleHTTPServer
```
Open
http://localhost:8000/js/scatterplot.html?file=[write to file].json"
You'll need to wait a bit for file to load

## Treemap of Class Hierarchy
To run the class hierarchy visualization
```
make class_hierarchy_driver
./class_hierarchy_driver traces/[file].hierarchy traces/[file].names json/[outfile].json < traces[file].trace

python -m SimpleHTTPServer
```
Open
"http://localhost:8000/js/treemap.html?file=[write to file].json"
You'll need to wait a bit for file to load

NOTE: there is some filtering and data processing occuring in javascript to clean up the visualization. This is specialized for the batik dataset, and will not necessarily work in the general case. 
