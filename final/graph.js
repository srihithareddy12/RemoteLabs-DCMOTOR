//fetch volatge,rpm values from thingspeak and pushing into arrays
async function fetchVal() {
  var xArray = [];
  var yArray = [];

  const response = await fetch(
    "https://api.thingspeak.com/channels/1922367/feeds.json?api_key=RM7XJZI5B5QF4MM8&results=10"
  );

  const datas = await response.json();
  for (let i = 0; i < datas.feeds.length; i++) {
    if (datas.feeds[i].field2) yArray.push(datas.feeds[i].field2); ///
    if (datas.feeds[i].field3) xArray.push(datas.feeds[i].field3);
  }

  var data = [
    {
      x: xArray,
      y: yArray,
      // mode: "lines",
      type: "scatter",
    },
  ];

  // Define Layout
  var layout = {
    xaxis: {
      title: "voltage",
      dtick: 1,
    },
    yaxis: { title: "RPM" },
    title: "Voltage vs. RPM",
  };

  // Display using Plotly
  Plotly.newPlot("myPlot", data, layout);
}
// fetchVal();
setInterval(fetchVal, 10000);
