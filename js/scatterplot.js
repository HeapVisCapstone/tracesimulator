var margin = {top: 30, right: 50, bottom: 40, left:40};
var width = 1400 - margin.left - margin.right;
var height = 800 - margin.top - margin.bottom;

var svg = d3.select('body')
    .append('svg')
    .attr("id", "#svg")
    .attr('width', width + margin.left + margin.right)
    .attr('height', height + margin.top + margin.bottom)
.append('g')
    .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');


var display = d3.select('body')
    .append('div')

// The API for scales have changed in v4. There is a separate module d3-scale which can be used instead. The main change here is instead of d3.scale.linear, we have d3.scaleLinear.
var xScale = d3.scaleLinear()
    .range([0, width]);

var yScale = d3.scaleLinear()
    .range([height, 0]);

// square root scale.
var radius = d3.scaleSqrt()
    .range([2,6]);

// the axes are much cleaner and easier now. No need to rotate and orient the axis, just call axisBottom, axisLeft etc.
var xAxis = d3.axisBottom()
    .scale(xScale);

var yAxis = d3.axisLeft()
    .scale(yScale);

// again scaleOrdinal
var color = d3.scaleOrdinal(d3.schemeCategory10);


// https://stackoverflow.com/questions/901115/how-can-i-get-query-string-values-in-javascript
function getParameterByName(name, url) {
    if (!url) url = window.location.href;
    name = name.replace(/[\[\]]/g, "\\$&");
    var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
        results = regex.exec(url);
    if (!results) return null;
    if (!results[2]) return '';
    return decodeURIComponent(results[2].replace(/\+/g, " "));
}

file = getParameterByName("file")


d3.json("../json/"+file).then( data => {

data = data.children;


data.forEach( d => {
    d.x = +d.x
    d.y = +d.y
    d.value = + d.rollup_data.size
    d.typecount = d.rollup_data.typecount

    // console.log(d)

    Object.entries(d.typecount).forEach(
        ([k, v]) => d.typecount[k] = +v)

})

console.log(data)

xScale.domain(d3.extent(data, function(d){
    return d.x;
})).nice();

yScale.domain(d3.extent(data, function(d){
    return d.y;
})).nice();

radius.domain(d3.extent(data, function(d){
    return d.value;
})).nice();


// adding axes is also simpler now, just translate x-axis to (0,height) and it's alread defined to be a bottom axis. 
svg.append('g')
    .attr('transform', 'translate(0,' + height + ')')
    .attr('class', 'x axis')
    .call(xAxis);

// y-axis is translated to (0,0)
svg.append('g')
    .attr('transform', 'translate(0,0)')
    .attr('class', 'y axis')
    .call(yAxis);


var bubble = svg.selectAll('.bubble')
    .data(data)
    .enter().append('circle')
    .attr('class', 'bubble')
    .attr('cx', function(d){return xScale(d.x);})
    .attr('cy', function(d){ return yScale(d.y); })
    .attr('r', function(d){ return radius(d.value); })
    .style('fill', function(d){ return color(0); });

bubble.append('title')
    .attr('x', function(d){ return radius(d.value); })
    .text(function(d){
        return "Count:"+d.value + "\n" + JSON.stringify(d.typecount);
    });

var pointStart = null
var pointEnd   = null

function fromAbsX(x) {
    return xScale.invert(x - margin.left)
}
function fromAbsY(y) {
    return yScale.invert(y - margin.top)
}
var inRange = (dim, range) => v => (range[0] <= v[dim] && v[dim] <= range[1])

var svgelement = document.getElementById("#svg");
svgelement.onmousedown = function(p) {
    // var p = d3.mouse(this);
    pointStart = {x : fromAbsX(p.clientX), y : fromAbsY(p.clientY) }
}

svgelement.onmouseup = function(p) {
    pointEnd = {x : fromAbsX(p.clientX), y : fromAbsY(p.clientY) }

    renderPoints()
}

function renderPoints() {
    var xRange = pointStart.x < pointEnd.x ? [pointStart.x, pointEnd.x]
                                           : [pointEnd.x,   pointStart.x] 

    var yRange = pointStart.y < pointEnd.y ? [pointStart.y, pointEnd.y]
                                           : [pointEnd.y,   pointStart.y]                                     

    var points = data.filter(inRange("x", xRange)).filter(inRange("y",  yRange))

    var mergeInto = (from, to) => {
        Object.entries(from).forEach(
            ([k, v]) => to.hasOwnProperty(k) ? to[k] = to[k] + v
                                             : to[k] = v )
        return to;
    }

    var alltypes = points.reduce( (acc, point) => mergeInto(point.typecount, acc)
                            , {})

    console.log("Types", "x", xRange, "y", yRange)

    console.log(alltypes)

    var html = ""
    Object.entries(alltypes).forEach( 
        ([k, v]) => html += "<p>"+k + ": " + v + "</p>")

    display.html(html)
}


})





/*
d3.csv('iris.csv').then( function(data){
    data.forEach(function(d){
         d.SepalLength = +d.SepalLength;
         d.SepalWidth = +d.SepalWidth;
         d.PetalLength = +d.PetalLength;
         d.Species = d.Name;
    });

    xScale.domain(d3.extent(data, function(d){
        return d.SepalLength;
    })).nice();

    yScale.domain(d3.extent(data, function(d){
        return d.SepalWidth;
    })).nice();

    radius.domain(d3.extent(data, function(d){
        return d.PetalLength;
    })).nice();

    // adding axes is also simpler now, just translate x-axis to (0,height) and it's alread defined to be a bottom axis. 
    svg.append('g')
        .attr('transform', 'translate(0,' + height + ')')
        .attr('class', 'x axis')
        .call(xAxis);

    // y-axis is translated to (0,0)
    svg.append('g')
        .attr('transform', 'translate(0,0)')
        .attr('class', 'y axis')
        .call(yAxis);


    var bubble = svg.selectAll('.bubble')
        .data(data)
        .enter().append('circle')
        .attr('class', 'bubble')
        .attr('cx', function(d){return xScale(d.SepalLength);})
        .attr('cy', function(d){ return yScale(d.SepalWidth); })
        .attr('r', function(d){ return radius(d.PetalLength); })
        .style('fill', function(d){ return color(d.Species); });

    bubble.append('title')
        .attr('x', function(d){ return radius(d.PetalLength); })
        .text(function(d){
            return d.Species;
        });

    // adding label. For x-axis, it's at (10, 10), and for y-axis at (width, height-10).
    svg.append('text')
        .attr('x', 10)
        .attr('y', 10)
        .attr('class', 'label')
        .text('Sepal Width');


    svg.append('text')
        .attr('x', width)
        .attr('y', height - 10)
        .attr('text-anchor', 'end')
        .attr('class', 'label')
        .text('Sepal Length');

    // I feel I understand legends much better now.
    // define a group element for each color i, and translate it to (0, i * 20). 
    var legend = svg.selectAll('legend')
        .data(color.domain())
        .enter().append('g')
        .attr('class', 'legend')
        .attr('transform', function(d,i){ return 'translate(0,' + i * 20 + ')'; });

    // give x value equal to the legend elements. 
    // no need to define a function for fill, this is automatically fill by color.
    legend.append('rect')
        .attr('x', width)
        .attr('width', 18)
        .attr('height', 18)
        .style('fill', color);

    // add text to the legend elements.
    // rects are defined at x value equal to width, we define text at width - 6, this will print name of the legends before the rects.
    legend.append('text')
        .attr('x', width - 6)
        .attr('y', 9)
        .attr('dy', '.35em')
        .style('text-anchor', 'end')
        .text(function(d){ return d; });


    // d3 has a filter fnction similar to filter function in JS. Here it is used to filter d3 components.
    legend.on('click', function(type){
        d3.selectAll('.bubble')
            .style('opacity', 0.15)
            .filter(function(d){
                return d.Species == type;
            })
            .style('opacity', 1);
    })


})

*/