import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.lines as lns
import numpy as np
import Figtodat
from images2gif import writeGif
import time as time

import StrategiesModule as Strat


def CreateAnimation(graph,numberOfRounds):
	N = numberOfRounds
	figure = plt.figure(1)
	plot = figure.add_subplot(111)
	plot.hold(False)
	images = []

	graphSquares = []
	for agent in graph.GetAgents():
		rect =  plt.Rectangle(agent.position, 1., 1., edgecolor = "black")
		graphSquares.append([agent,rect])
		plt.gca().add_artist(rect)
	plt.axis([0, graph.GetWidth(), 0, graph.GetHeight()])
	plt.axes().set_aspect(1)

	for i in range(N):
		print i
		#plt.clf()
		#plt.axes().set_aspect(1)
		for graphSquare in graphSquares:
			graphSquare[1].set_facecolor(graphSquare[0].GetStrategyHistory()[i].GetColour())

		im = Figtodat.fig2img(figure)
		images.append(im)
	writeGif("animations\images.gif", images, duration = 0.3, dither = 0)


def AnimateGraphWithEdges(graph, numberOfRounds):
	fig = plt.figure()
	ax = fig.add_subplot(111, aspect = 'equal', autoscale_on = True)
	round_text = ax.text(0.02, 0.95, '', transform = ax.transAxes)

	graphSquares = []
	for agent in graph.GetAgents():
		rect = plt.Rectangle(agent.position, 1., 1., facecolor = "blue", edgecolor = "black")
		ax.add_artist(rect)
		graphSquares.append(rect)

	graphLines = []
	for edge in graph.GetEdges():
		agent1 = edge.Agent1()
		agent2 = edge.Agent2()
		position1 = np.array(agent1.position) + np.array([0.5,0.5])
		position2 = np.array(agent2.position) + np.array([0.5,0.5])
		deltaPosition = position2 - position1
		if abs(deltaPosition[0]) * 2 > graph.GetWidth():
			if deltaPosition[0] > 0:
				deltaPosition[0] -= graph.GetWidth()
			else:
				deltaPosition[0] += graph.GetWidth()

		if abs(deltaPosition[1]) * 2 > graph.GetHeight():
			if deltaPosition[1] > 0:
				deltaPosition[1] -= graph.GetWidth()
			else:
				deltaPosition[1] += graph.GetWidth()

		line1 = lns.Line2D(xdata = [(position1 + deltaPosition*0.25)[0],(position1 + deltaPosition*0.5)[0]], ydata = [(position1 + deltaPosition*0.25)[1],(position1 + deltaPosition*0.5)[1]])
		line2 = lns.Line2D(xdata = [(position2 - deltaPosition*0.25)[0],(position2 - deltaPosition*0.5)[0]], ydata = [(position2 - deltaPosition*0.25)[1],(position2 - deltaPosition*0.5)[1]])
		ax.add_artist(line1)
		ax.add_artist(line2)
		graphLines.append([line1,line2])

	numberOfAgents = len(graph.GetAgents())
	numberOfEdges = len(list(graph.GetEdges()))

	squareColours = np.empty((numberOfAgents, numberOfRounds), dtype = object)
	lineColours = np.empty((numberOfEdges,2, numberOfRounds), dtype = object)

	for i in range(numberOfAgents):
		agent = graph.GetAgents()[i]
		agentColours = [strategy.GetColour() for strategy in agent.GetStrategyHistory()[0:numberOfRounds]]
		for j in range(numberOfRounds):
			squareColours[i, j] = agentColours[j]

	for i in range(numberOfEdges):
		edge = list(graph.GetEdges())[i]
		edgeColours = [[Strat.GetMoveColour(moves[0]), Strat.GetMoveColour(moves[1])] for moves in edge.GetHistory()[0:numberOfRounds]]
		for j in range(numberOfRounds):
			lineColours[i, 0, j] = edgeColours[j][0]
			lineColours[i, 1, j] = edgeColours[j][1]

	print 'Data pulled. Creating animation'

	points_ani = animation.FuncAnimation(fig, PlotGraphWithEdges, numberOfRounds, fargs = (graph, graphSquares, squareColours, graphLines, lineColours, round_text,ax),
										 interval = 250, blit = False)
	plt.show()
	return points_ani


def PlotGraphWithEdges(roundNumber, graph,graphSquares,squareColours,graphLines,lineColours,round_text,ax):
	for i in range(len(graphSquares)):
		graphSquares[i].set_facecolor(squareColours[i,roundNumber])
	for i in range(len(graphLines)):
		graphLines[i][0].set_color(lineColours[i,0,roundNumber])
		graphLines[i][1].set_color(lineColours[i,1,roundNumber])
	round_text.set_text('round = ' + str(roundNumber))
	ax.axis([0, graph.GetWidth(), 0, graph.GetHeight()])
	patches = graphSquares + graphLines + [round_text]
	return patches


def AnimateGraph(graph, numberOfRounds):
	if numberOfRounds > len(graph.GetScoreHistory()):
		raise ValueError("Cannot Animate for more frames than the graph has been played for.")
	fig = plt.figure()
	ax = fig.add_subplot(111, aspect = 'equal', autoscale_on = True)
	round_text = ax.text(0.02, 0.95, '', transform = ax.transAxes)

	graphSquares = []
	for agent in graph.GetAgents():
		rect =  plt.Rectangle(agent.position, 1., 1.,facecolor = "blue", edgecolor = "black")
		ax.add_artist(rect)
		graphSquares.append(rect)

	numberOfAgents = len(graph.GetAgents())

	colours = np.empty((numberOfAgents, numberOfRounds),  dtype = object)
	for i in range(numberOfAgents):
		agent = graph.GetAgents()[i]
		agentColours = [strategy.GetColour() for strategy in agent.GetStrategyHistory()[0:numberOfRounds]]
		for j in range(numberOfRounds):
			colours[i,j] = agentColours[j]

	points_ani = animation.FuncAnimation(fig, PlotGraph, numberOfRounds, fargs = (graph,graphSquares,colours,round_text,ax),
										 interval = 250, blit = False)
	plt.show()
	return points_ani


def PlotGraph(roundNumber, graph,graphSquares,colours,round_text,ax):
	for i in range(len(graphSquares)):
		graphSquares[i].set_facecolor(colours[i,roundNumber])
	round_text.set_text('round = ' + str(roundNumber))
	ax.axis([0, graph.GetWidth(), 0, graph.GetHeight()])
	patches = graphSquares + [round_text]
	return patches


def SaveAnimation(animation,filename):
	startTime = time.time()
	animation.save('animations\\' + filename + '.mp4', bitrate = 8000)
	endTime = time.time()
	deltaTime = endTime - startTime
	print 'Saving animation took '+ str(deltaTime) + 's'


def PlotRectGraph(graph,numberOfRounds):
	N = numberOfRounds
	for i in range(N):
		print i
		plt.figure(i)
		plt.clf()
		plt.axes().set_aspect(1)

		agents = graph.GetAgents()
		for agent in agents:
			strategy = agent.GetStrategyHistory()[i]
			graphSquare = plt.Rectangle(agent.position,1.,1.,facecolor = strategy.GetColour(),edgecolor = "black")
			plt.gca().add_artist(graphSquare)
		plt.axis([0,graph.GetWidth(),0,graph.GetHeight()])
		plt.show()
