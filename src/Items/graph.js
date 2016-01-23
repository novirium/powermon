	google.load('visualization', '1', {packages: ['corechart', 'line']});
	google.setOnLoadCallback(function() {
		$(function() {
			updateGraph();
		});
	});
	
	function updateGraph() {
		$.ajax({
			url: 'values.json', 
			success: function(data) {
				drawGraph(organiseData(data))
			},
			complete: function() {
				setTimeout(updateGraph, 1000);
			}
		});
	}
	
	function organiseData(data) {
		var dataOut;
		var i;
		
		dataOut = new Array();
		for (i=0; i<data.length; i++) {
			dataOut[i] = new Array();
			dataOut[i][0] = i;
			dataOut[i][1] = data[i];
		}
		
		return dataOut;
	}
	
	function drawGraph(data) {
	
		var dataIn = new google.visualization.DataTable();
		dataIn.addColumn('number', 'X');
		dataIn.addColumn('number', 'Value');
		dataIn.addRows(data);

		var options = {
			hAxis: {
				title: 'Time'
			},
			vAxis: {
				title: 'Value'
			}
		};

		var chart = new google.visualization.LineChart(document.getElementById('chart_div'));

		chart.draw(dataIn, options);
	}
