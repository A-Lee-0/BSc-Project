"""Graph module
Andrew Lee 19/01/2017
This Module exists to create and use the GraphNode class for use in our 2017 game theory project with Dave Clements and Sam Kirby."""


import numpy as np
import weakref as wr

import StrategiesModule as Strat
reload(Strat)
#import time as time
#from collections import deque

scoreGrid = {'dd': [2, 2], 'dc': [0, 3],
			 'cd': [3, 0], 'cc': [1, 1]}											#Create Score grid for the game. Dictionary of 'p1p2' moves to list with first element = p1 jail time, second element = p2 jail time.


def UpdateScoreGrid(dd = None, dc = None, cd = None, cc = None):
	if not (dd is None):
		scoreGrid.update({'dd': dd})
	if not (dc is None):
		scoreGrid.update({'dc': dc})
	if not (cd is None):
		scoreGrid.update({'cd': cd})
	if not (cc is None):
		scoreGrid.update({'cc': cc})


def Score(moves):
	[move1, move2] = moves
	return scoreGrid[move1+move2]


def LoadGraph(filename):
	"""Loads text file containing agent IDs and strategies, and edges between agents"""
	f = open(filename,'r')
	name = f.readline()[:-1]
	newGraph = Graph(name)
	print 'Created Graph'
	line = f.readline()
	while line != '':
		agentID = line[0:line.find(':')]
		edges = line[4:len(line)-1]
		strategyString = edges[edges.find(':')+2:]
		agent = Agent(agentID, graph = newGraph)
		agent.SetStrategy(strategyString)
		edges = edges[0:edges.find(':')]
		while edges != '':
			linkedAgentID = edges[0:2]
			linkedAgent = next((x for x in Agent.agents if x.GetID() == linkedAgentID), '')
			if linkedAgent != '':
				agent.CreateEdge(linkedAgent)
			edges = edges[3:]
		line = f.readline()
	return newGraph


class Graph:
	graphs = []

	def __init__(self, name = ''):
		self.__agents = []
		self.__edges = set()
		self.__name = name
		self.__id = len(Graph.graphs)
		self.__totalScore = 0
		self.__scoreHistory = []
		Graph.graphs.append(self)

	def PlayRound(self, gamesPerRound = 1):
#		startTime = time.time()

		#Play the game between each pair of connected agents.
		for i in range(gamesPerRound):
			for edge in self.__edges:
				edge.PlayGame()

		#Move round scores and moves to history.
		for agent in self.__agents:
			agent.EndRound()
		self.__scoreHistory.append(self.__totalScore)

		#Update each agent's strategy if there is no neighbour worse than them.
		for agent in self.__agents:
			agent.UpdateStrategy()

#		endTime = time.time()
#		deltaTime = endTime - startTime
#		print 'Round took '+ str(deltaTime) + 's'

	def PlayNRounds(self,N, gamesPerRound = 1, printRoundNo = True):
		for i in range(N):
			if printRoundNo == True:
				print i
			self.PlayRound(gamesPerRound)

	def AddEdge(self,edge):
		self.__edges.add(edge)
	def AddAgent(self,agent):
		self.__agents.append(agent)
	def AddToTotalScore(self,score):
		self.__totalScore += score
	def GetEdges(self):
		return self.__edges
	def GetAgents(self):
		return self.__agents
	def GetScoreHistory(self):
		return self.__scoreHistory
	def CountStrategies(self):
		strategyCounts = []
		strategies = [agent.strategy.stratString for agent in self.__agents]
		for stratString in Strat.strategies:
			noOfStrat = strategies.count(stratString)
			strategyCounts.append([stratString,noOfStrat])
		return strategyCounts


class RectangleGraph(Graph):
	def __init__(self,width = 10,height = 10):
		Graph.__init__(self)
		self.__width = width
		self.__height = height
		self.__agentArray = np.empty([width,height], dtype = object)
		for i in range(width):
			for j in range(height):
				agent = Agent(str(i) + ',' + str(j),self)
				agent.position = [i,j]
				self.__agentArray[i][j] = agent
		for i in range(width):
			for j in range(height):
				agent = self.__agentArray[i][j]
				agent2 = self.__agentArray[np.mod(i+1,width)][j]
				GraphEdge(agent,agent2)
				agent2 = self.__agentArray[np.mod(i-1,width)][j]
				GraphEdge(agent, agent2)
				agent2 = self.__agentArray[i][np.mod(j+1,height)]
				GraphEdge(agent, agent2)
				agent2 = self.__agentArray[i][np.mod(j-1,height)]
				GraphEdge(agent, agent2)
				agent2 = self.__agentArray[np.mod(i-1,width)][np.mod(j+1,height)]
				GraphEdge(agent, agent2)
				agent2 = self.__agentArray[np.mod(i-1,width)][np.mod(j-1,height)]
				GraphEdge(agent, agent2)
				agent2 = self.__agentArray[np.mod(i+1,width)][np.mod(j+1,height)]
				GraphEdge(agent, agent2)
				agent2 = self.__agentArray[np.mod(i+1,width)][np.mod(j-1,height)]
				GraphEdge(agent, agent2)

	def GetWidth(self):
		return self.__width

	def GetHeight(self):
		return self.__height

nullGraph = Graph('')						#Created to make it easier to access Graph methods in the subsequent classes - messy alternative to setting the type of the parameter.


class Agent:
#	agents = []
	def __init__(self,agentID, graph = nullGraph, strategy = None):
		self.position = []
		self.connectedAgents = []
		if strategy is None:
			self.strategy = Strat.Strategy(self)
		else:
			self.strategy = strategy
		self.oldStrategy = self.strategy
		self.roundScore = 0
		self.__scores = []
		self.__edges = set()
		self.__strategyHistory = []
		self.__id = agentID
		self.__graph = wr.ref(graph)
		self.__movesAgainst = []
		self.__movesAgainstHistory = []
		graph.AddAgent(self)
#		Agent.agents.append(self)

	def CreateEdge(self,agent):
		newEdge = GraphEdge(self,agent)
#		self.__edges.add(newEdge)			#Add edge to list of edges
		if self.__graph() == nullGraph:
			raise ValueError('Warning: Tried to add an edge to an agent with no graph!')
		else:
			self.__graph().AddEdge(newEdge)			#Add edge to the parent graph.

	def AddEdge(self,edge):
		self.__edges.add(edge)
		if self.__graph() == nullGraph:
			raise ValueError('Warning: Tried to add an edge to an agent with no graph!')
		else:
			self.__graph().AddEdge(edge)			#Add edge to the parent graph.

	def ResetConnectedAgents(self):
		"""Regenerates the list of connected agents for this agent from this agent's __edges list.
		Returns the new list."""
		self.connectedAgents = []
		for edge in self.__edges:
			self.connectedAgents.append(wr.ref(edge.OtherAgent(self)))
		return self.connectedAgents

	def GetLowestScoringConnectedAgent(self):
		"""Returns the connected agent with the Lowest score in the last round played.
		If this agent ties that score, returns this agent."""
		connectedAgentScores = [[agentRef().GetScores()[-1], agentRef()] for agentRef in self.connectedAgents]
		lowestScore = min(connectedAgentScores)
		if lowestScore[0] < self.__scores[-1]:
			return lowestScore[1]
		else:
			return self

	def GetHighestScoringConnectedAgent(self):
		"""Returns the connected agent with the Highest score in the last round played.
		If this agent ties that score, returns this agent."""
		connectedAgentScores = [[agentRef().GetScores()[-1], agentRef()] for agentRef in self.connectedAgents]
		highestScore = max(connectedAgentScores)
		if highestScore[0] > self.__scores[-1]:
			return highestScore[1]
		else:
			return self

	def GetMinMaxScoringConnectedAgents(self):
		"""Returns a list of the connected agents with the Highest and Lowest scores in the last round played.
		If this agent ties either score, returns this agent.
		[Highest,Lowest]"""
		connectedAgentScores = map(GetConnectedScores, self.connectedAgents)
#		connectedAgentScores.insert(0,[self.GetScores()[-1],self])
		selfScore = self.GetScores()[-1]
		highestScore = max(connectedAgentScores)
		lowestScore = min(connectedAgentScores)
		if highestScore[0] == selfScore:
			if lowestScore[0] == selfScore:
				return [self,self]
			else:
				return [self,lowestScore[1]]
		if lowestScore[0] == selfScore:
			return [highestScore[1],self]
		return [highestScore[1],lowestScore[1]]

	def SetStrategy(self,stratString):
		"""Sets the agent's Strategy to a new strategy specified by the strategy string.
		Does not update this agent's strategyHistory list"""
		self.strategy = Strat.GetStrategy(self, stratString)

	def UpdateStrategy(self):
		"""If no connected agent scores higher than this agent, set strategy to match lowest scoring neighbour.
		Appends old strategy to __strategyHistory list regardless."""
		self.__strategyHistory.append(self.strategy)
		highestAgent,lowestAgent = self.GetMinMaxScoringConnectedAgents()
		if self == highestAgent:
			stratString = lowestAgent.oldStrategy.stratString 	#Get the String needed to create a new strategy of the same type.
			self.strategy = Strat.GetStrategy(self, stratString)		#Create a new strategy of the best adjacent type for the current agent.

	def AddToGraph(self,graph):
		if self.__graph() != nullGraph:
			print ('Warning: Agent ID ' + self.__id + ' is already part of a graph! Why are we changing its graph?')
		self.__graph = wr.ref(graph)					#Sets the parent graph for this agent.
		graph.AddAgent(self)

	def AddMoveAgainst(self,move,agent):
		self.__movesAgainst.append([move,wr.ref(agent)])

	def EndRound(self):
		self.__scores.append(self.roundScore)					#Move roundScore to history and reset
		self.__graph().AddToTotalScore(self.roundScore)
		self.roundScore = 0

		self.__movesAgainstHistory.append(self.__movesAgainst)	#Move movesAgainst to history and reset
		self.__movesAgainst = []

		self.oldStrategy = self.strategy						#saves a copy of the current strategy, so the UpdateStrategy method updates to the most successful strategy that it can see, rather than the most successful strategy it's most successful neighbor can see.
																#alternative to this line would be to index into the __strategyHistory list, and ensure you're looking at the right entry that way.
	def GetScores(self):
		return self.__scores
	def GetColour(self):
		return self.strategy.GetColour()
	def GetEdges(self):
		return self.__edges						#Returns the list of connected agents to this agent.`
	def GetStrategy(self):
		return self.strategy					#Returns the current strategy of this agent.
	def GetStrategyHistory(self):
		return self.__strategyHistory			#Returns the previous strategies of this agent.
	def GetMovesAgainstHistory(self):
		return self.__movesAgainstHistory
	def GetID(self):
		return self.__id						#Returns the ID of this agent.
	def GetGraph(self):
		return self.__graph()						#Returns the parent Graph of this agent.
	def __gt__(self,agent):
		return self.__id > agent.GetID()		#Defines the > operator for the agent to use the agent's ID string.
	def __lt__(self,agent):
		return self.__id < agent.GetID()		#Defines the < operator for the agent to use the agent's ID string.

def GetConnectedScores(agentRef):
	return [agentRef().GetScores()[-1],agentRef()]

class GraphEdge(tuple):
#	edges = set()
	def __new__(cls, agent1, agent2):
		if agent1.GetGraph() != agent2.GetGraph():
			raise ValueError('Error: Agents on edge have different graphs! Do we want to change code to merge graphs?')
		wr1 = wr.ref(agent1)
		wr2 = wr.ref(agent2)
		if agent1 > agent2:						#Ensure edge object has agents in the correct order.
			agentTuple = tuple.__new__(cls, (wr2, wr1))
		else:
			agentTuple = tuple.__new__(cls, (wr1, wr2))
		return agentTuple

	def __init__(self,agent1,agent2):
		self.__history = []
#		GraphEdge.edges.add(self)
		agent1.AddEdge(self)
		agent2.AddEdge(self)
		agent1.connectedAgents.append(wr.ref(agent2))
		agent2.connectedAgents.append(wr.ref(agent1))

	def Agent1(self):
		return self[0]()

	def Agent2(self):
		return self[1]()

	def OtherAgent(self,agent):
		if self[0]() == agent:
			return self[1]()
		elif self[1]() == agent:
			return self[0]()
		else:
			return ''

	def GetAgentIndex(self,agent):
		if self[0]() == agent:
			return 0
		elif self[1]() == agent:
			return 1
		else:
			raise ValueError('Error: Looked for agent index on edge that does not contain the agent!')

	def GetHistory(self):
		return self.__history

	def AddToHistory(self,result):
		self.__history.append(result)
		return self.__history

	def PlayGame(self):
		a1 = self[0]()
		a2 = self[1]()

		#Get moves played by agents
		move1 = a1.GetStrategy().PlayRound(self,0)
		move2 = a2.GetStrategy().PlayRound(self,1)

		#Get score of agent's moves from the score matrix.
		score = Score([move1,move2])

		#Increment agent's round scores by edge results.
		a1.roundScore += score[0]
		a2.roundScore += score[1]

		#Save game moves to both agents.
		a1.AddMoveAgainst(move2,a2)
		a2.AddMoveAgainst(move1,a1)

		#Save game moves to own history.
		self.AddToHistory([move1,move2])
		return score

