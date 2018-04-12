var margin = {top: 20, right: 0, bottom: 0, left: 0},
    width = 960,
    height = 960 - margin.top - margin.bottom,
    transitioning;

// Domain set dynamically
var xScale = d3.scaleLinear()
    .range([0, width]);
var yScale = d3.scaleLinear()
    .range([0, height]);

var svg = d3.select("#chart").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.bottom + margin.top)
    .style("margin-left", -margin.left + "px")
    .style("margin.right", -margin.right + "px")
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")")
    .style("shape-rendering", "crispEdges");

var grandparent = svg.append("g")
    .attr("class", "grandparent");

grandparent.append("rect")
    .attr("y", -margin.top)
    .attr("width", width)
    .attr("height", margin.top);

grandparent.append("text")
    .attr("x", 6)
    .attr("y", 6 - margin.top)
    .attr("dy", ".75em");

// traverses tree to fill in 
function fillData(tree) {
    var traverse = (d, p) => node => {
        var n = {}
        n.parent = p
        n.depth = d
        n.x  = parseInt(node.x)
        n.dx = parseInt(node.dx)
        n.y  = parseInt(node.y)
        n.dy = parseInt(node.dy)
        n.id = node.id
        n.data = node.rollup_data

        if (node.children)
            n.children = node.children.map(traverse(d+1, n));

        return n

    }
    return traverse(0, null)(tree);
}

var parentOf = d => d.parent


var fullnameOf = d => d.parent ? fullnameOf(d.parent) + "." + d.id
                                 : d.id
var nameOf = d => d.id.substring(0, 5)

d3.json("../json/small.json").then( root => {
    console.log(root)
    data = fillData(root)
    console.log(data)
    xScale.domain([data.x, data.x + data.dx])
    yScale.domain([data.y, data.y + data.dy])

    display(data)
});

  function display(d) {
    console.log("launching display")


    grandparent
        .datum(parentOf(d))
        .on("click", transition)
      .select("text")
        .text(fullnameOf(d));


    var g1 = svg.insert("g", ".grandparent")
        .datum(d)
        .attr("class", "depth");

    var g = g1.selectAll("g")
        .data(d.children)
      .enter().append("g");

    g.filter(function(d) { return d.children; })
        .classed("children", true)
        .on("click", transition);

    g.selectAll(".child")
        .data(function(d) { return d.children || [d]; })
      .enter().append("rect")
        .attr("class", "child")
        .call(rect);

    g.append("rect")
        .attr("class", "parent")
        .call(rect)
      .append("title")
        .text((d) => 5);

    g.append("text")
        .attr("dy", ".75em")
        .text(nameOf)
        .call(text);

    function transition(d) {
      console.log("transitionign to ", d)

      if (transitioning || !d) return;
      transitioning = true;

      var g2 = display(d),
          t1 = g1.transition().duration(750),
          t2 = g2.transition().duration(750);

      // Update the domain only after entering new elements.
      xScale.domain([d.x, d.x + d.dx]);
      yScale.domain([d.y, d.y + d.dy]);

      // Enable anti-aliasing during the transition.
      // svg.style("shape-rendering", null);

      // Draw child nodes on top of parent nodes.
      svg.selectAll(".depth").sort(function(a, b) { return a.depth - b.depth; });

      // Fade-in entering text.
      g2.selectAll("text").style("fill-opacity", 0);

      // Transition to the new view.
      t1.selectAll("text").call(text).style("fill-opacity", 0);
      t2.selectAll("text").call(text).style("fill-opacity", 1);
      t1.selectAll("rect").call(rect);
      t2.selectAll("rect").call(rect);

        transitioning = false;
      // // Remove the old node when the transition is finished.
      t1.remove()
      // svg.style("shape-rendering", "crispEdges");
      // .each("end", function() {
      // });
    }

    return g;

  }

  function text(text) {
    text.attr("x", function(d) { return xScale(d.x) + 6; })
        .attr("y", function(d) { return yScale(d.y) + 6; });
  }

  function rect(rect) {
    rect.attr("x", function(d) { return xScale(d.x); })
        .attr("y", function(d) { return yScale(d.y); })
        .attr("width", function(d) { return xScale(d.x + d.dx) - xScale(d.x); })
        .attr("height", function(d) { return yScale(d.y + d.dy) - yScale(d.y); });
  }

