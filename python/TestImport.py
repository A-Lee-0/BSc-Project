import GraphModule as GM
import AnimationModule as AM
reload(GM)
reload(AM)
import os


selfDirectory = os.path.dirname(__file__)
SGCBDirectory = os.path.dirname(selfDirectory)

def LoadSavedGraph(filename):
	# file = '\dump[\]TestGrid.txt'
	file = '\dump\\' + filename

	filepath = SGCBDirectory + file
	print 'Filepath: ' + filepath


	graph = GM.LoadGraph(filepath)
	return graph


def RandomRectGraph(width,height):
	"""Create a rectangular graph of agents, where each agent has a randomly selected initial strategy."""
	graph = GM.RectangleGraph(width, height)						#create rectangular graph of width and height specified with each agent connected to surrounding 8 agents.
	for agent in graph.GetAgents():
		agent.SetStrategy('RandomStrategy')
	return graph

def NRectGraphsForMRounds(noOfGraphs,noOfRounds,width,height, returnGraphs = False):
	"""Create N random rectangular graphs, and run them for M rounds. Then take the count of each strategy remaining at the end.
	Returns Nx2 list of graphs and their respective counts."""
	graphs = []
	counts = []
	for i in range(noOfGraphs):
		print i
		graph = RandomRectGraph(width, height)
		graph.PlayNRounds(noOfRounds,printRoundNo = False)
		countOfStrategies = graph.CountStrategies()
		if returnGraphs == True:
			graphs.append(graph)
		else:
			GM.Graph.graphs.pop()		#clear graph from list of graphs so it doesn't burn memory.
		counts.append(countOfStrategies)
	if returnGraphs == True:
		return counts, graphs
	return counts


graph = RandomRectGraph(20,20)
graph.PlayNRounds(40)
#graphAnimation = AM.AnimateGraph(graph,40)
