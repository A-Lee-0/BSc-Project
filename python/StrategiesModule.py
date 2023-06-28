import random
import weakref as wr

strategies = ['D','C','R','TT','DTT','TTT']

def GetStrategy(agent,stratString):
	if stratString == 'D':
		return Defect(agent)
	elif stratString == 'C':
		return Cooperate(agent)
	elif stratString == 'R':
		return Random(agent)
	elif stratString == 'TT':
		return TitForTat(agent)
	elif stratString == 'DTT':
		return DistributedTitForTat(agent)
	elif stratString == 'TTT':
		return TitForTitForTat(agent)
	elif stratString == 'RandomStrategy':
		rand = random.randint(0,len(strategies)-1)
		return GetStrategy(agent, strategies[rand])
	else:
		raise ValueError("String supplied to GetStrategy method does not match any existing Strategies!")


def RandomMove():
	rand = random.random()
	if rand > 0.5:
		return 'c'
	else:
		return 'd'

def GetMoveColour(move):
	if move == 'd':
		return 'red'
	else:
		return 'blue'

class Strategy:
	strategies = []

	def __init__(self,agent):
		self.name = ''
		self.colour = ''
		Strategy.strategies.append(self)
		self.agent = wr.ref(agent)

	def PlayRound(self,edge,selfIndex):
		raise NotImplemented()

	def GetColour(self):
		return self.colour

	def GetAgent(self):
		return self.agent()

	def __str__(self):
		return self.name

class Defect(Strategy):
	def __init__(self,agent):
		Strategy.__init__(self,agent)
		self.name = 'Defect'
		self.stratString = 'D'

	def PlayRound(self,edge,selfIndex):
		return 'd'

	def GetColour(self):
		return 'r'


class Cooperate(Strategy):
	def __init__(self,agent):
		Strategy.__init__(self,agent)
		self.name = 'Cooperate'
		self.colour = "blue"
		self.stratString = 'C'

	def PlayRound(self,edge,selfIndex):
		return 'c'


class Random(Strategy):
	"""Strategy that randomly Cooperates half the time, and Defects half the time."""
	def __init__(self,agent):
		Strategy.__init__(self,agent)
		self.name = 'Random'
		self.colour = "white"
		self.stratString = 'R'

	def PlayRound(self,edge,selfIndex):
		return RandomMove()


class TitForTat(Strategy):
	def __init__(self,agent):
		Strategy.__init__(self,agent)
		self.name = 'TitForTat'
		self.colour = "purple"
		self.stratString = 'TT'

	def PlayRound(self,edge,selfIndex):
		if len(edge.GetHistory()) >= 1:
			lastRound = edge.GetHistory()[-1]
			agentIndex = not selfIndex
			if lastRound[agentIndex] == 'd':		#If move against agent last round was defect, then defect.
				return 'd'
			else:
				return 'c'
		else:
			return 'c'


class DistributedTitForTat(Strategy):
	def __init__(self,agent):
		Strategy.__init__(self,agent)
		self.name = 'DistributedTitForTat'
		self.colour = "magenta"
	 	self.stratString = 'DTT'

	def PlayRound(self,edge,selfIndex):
		movesAgainstHistory = self.GetAgent().GetMovesAgainstHistory()
		if len(movesAgainstHistory) > 0:
			lastRoundMoves = movesAgainstHistory[-1]
			rand = random.randint(0, len(lastRoundMoves) - 1)
			return lastRoundMoves[rand][0]
		return 'c'


class TitForTitForTat(Strategy):
	def __init__(self,agent):
		Strategy.__init__(self,agent)
		self.name = 'TitForTitForTat'
		self.colour = "cyan"
		self.stratString = 'TTT'

	def PlayRound(self,edge,selfIndex):
		if len(edge.GetHistory()) >= 2:
			lastRound = edge.GetHistory()[-1]
			lastLastRound = edge.GetHistory()[-2]
			#selfIndex = edge.GetAgentIndex(self)
			agentIndex = not selfIndex
#			moveAgainst = lastRound[agentIndex]
#			prevMoveAgainst = lastLastRound[agentIndex]
#			if moveAgainst == 'd' and prevMoveAgainst == 'd':
			if lastRound[agentIndex] == 'd' and lastLastRound[agentIndex]== 'd':
				return 'd'
			else:
				return 'c'
		else:
			return 'c'