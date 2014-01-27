-- Constants.
MAX_VELOCITY = 5.0
CAMERA_VELOCITY = 5.0
BASE_HEIGHT = 60
NPC_TALK_RADIUS = 200

function handleButton(button)
	if currLevel == 2 then
		if button == 1 then
			print("Throw rock")
		elseif button == 2 then
			print("Call out")
		end
		text:setText("We are sorry, but the game stops here.")
		text:setSize(40)
		waitForButton = false
	end
end

function startAnimation(level)
	animation = true
	if level == 2 then
		waitForButton = true
		moveCamera = true
		moveCameraFrom = 0.0
		moveCameraTo = bckgWidth * ratio * 0.43
		moveCameraProgress = 0.0
	end
end

function updateAnimation()
	if moveCamera then
		moveCameraProgress = moveCameraProgress + CAMERA_VELOCITY + timeDelta * ratio
		if moveCameraProgress > moveCameraTo then
			moveCameraProgress = moveCameraTo
			moveCamera = false
		end
		background:setPosX(-moveCameraProgress)
		player:setPosX(playerPosX - moveCameraProgress)
		if npc ~= nil then
			npc:setPosX(npcPosX - moveCameraProgress)
		end
	end
	if not moveCamera or not waitForButton then
		animation = false
	end
end

function updateText()
	if npc == nil then
		return
	end
	local distance = npc:getPosX() - player:getPosX()
	if distance > -NPC_TALK_RADIUS and distance < NPC_TALK_RADIUS then
		textNode:setVisibility(true)
	else
		textNode:setVisibility(false)
		return
	end
end

-- Setup level.
function setupLevel(level)
	print("Setting up level: " .. level)
	currLevel = level
	if background ~= nil then
		background:setVisibility(false)
		parallax:setVisibility(false)
	end
	if npc ~= nil then
		npc:setVisibility(false)
	end
	if level == 1 then
		textNode:setVisibility(false)
		portalRight = 2
		portalLeft = 0
		background = getNode("background")
		parallax = getNode("parallax")
		npc = nil
		if level1Width == nil then
			level1Width = background:getScaleX()
			level1Height = background:getScaleY()
		end
		bckgHeight = level1Height
		bckgWidth = level1Width
		leftOffset = 0
		rightOffset = bckgWidth
		resize()
		if lastLevel == 2 then
			playerPosX = bckgWidth * ratio - playerWidth * ratio / 2 - 1;
			player:setScaleX(-playerWidth * ratio)
		else
			playerPosX = playerWidth * ratio + 1;
		end
		text:setText("")
	elseif level == 2 then
		textNode:setVisibility(true)
		portalRight = 0
		portalLeft = 1
		background = getNode("cave")
		if level2Width == nil then
			level2Width = background:getScaleX()
			level2Height = background:getScaleY()
		end
		bckgHeight = level2Height
		bckgWidth = level2Width
		leftOffset = 0
		rightOffset = bckgWidth * 0.60
		resize()
		npc = getNode("npc")
		npc:setScaleX(700 * ratio)
		npc:setScaleY(700 * ratio)
		npcPosX = bckgWidth * ratio * 0.60
		npc:setPosY(BASE_HEIGHT * ratio)
		playerPosX = playerWidth * ratio + 1.0;
		text:setText("Press Q to throw a rock at him.\nPress W to come closer.")
		text:setSize(12)
		startAnimation(2)
	end
	if npc ~= nil then
		npc:setVisibility(true)
	end
	player:setPosY(BASE_HEIGHT * ratio)
	background:setVisibility(true)
	parallax:setVisibility(true)
	velocityX = 0
	lastLevel = level
	updatePositions()
end

function moveLeft()
	if facingRight then
		facingRight = false
		player:setScaleX(-playerWidth * ratio)
		posOffset = playerWidth * ratio
	end
	velocityX = velocityX - timeDelta * ratio
end

function moveRight()
	if not facingRight then
		facingRight = true
		player:setScaleX(playerWidth * ratio)
		posOffset = 0
	end
	velocityX = velocityX + timeDelta * ratio
end

-- Update character and background movement.
function updatePositions()
	-- Update x position.
	if velocityX > 0 then
		velocityX = velocityX - timeDelta * ratio * 0.5
		if velocityX < 0 then
			velocityX = 0
		end
	elseif velocityX < 0 then
		velocityX = velocityX + timeDelta * ratio * 0.5
		if velocityX > 0 then
			velocityX = 0
		end
	end
	playerPosX = playerPosX + velocityX
	if playerPosX > rightOffset * ratio - playerWidth * ratio / 2 then
		playerPosX = rightOffset * ratio  - playerWidth * ratio / 2
		velocityX = 0
		if portalRight > 0 then
			setupLevel(portalRight)
			return
		end
	elseif playerPosX < leftOffset * ratio + playerWidth * ratio / 2 then
		playerPosX = leftOffset * ratio + playerWidth * ratio / 2
		velocityX = 0
		if portalLeft > 0 then
			setupLevel(portalLeft)
			return
		end
	end
	-- Update background and parallax.
	if playerPosX < screenWidth / 2 then
		background:setPosX(0)
		parallax:setPosX(0)
		player:setPosX(playerPosX - playerWidth * ratio / 2 + posOffset)
	elseif playerPosX > background:getScaleX() - screenWidth / 2 then
		background:setPosX(-background:getScaleX() + screenWidth)
		parallax:setPosX(background:getPosX() / 2)
		player:setPosX(playerPosX - background:getScaleX() + screenWidth - playerWidth * ratio / 2 + posOffset)
	else
		background:setPosX(-playerPosX + screenWidth / 2)
		parallax:setPosX(background:getPosX() / 2)
		player:setPosX(screenWidth / 2 - playerWidth * ratio / 2 + posOffset)
	end
	if npc ~= nil then
		delta = player:getPosX() - posOffset + npcPosX
		npc:setPosX(delta - playerPosX)
	end
end

-- Called when network response is available.
function eventResponse(response)
end

-- Called when GUI component listeners are activated.
function eventGUI(guiElement, eventType)
end

-- Handle initialization.
function start()
	print("Called start()")
	playSound("sound.mp3", true)
	camera = getCamera()
	input = getInput()
	player = getNode("player")
	
	-- scenarioManager = getScenarioManager()
	
	
	-- scenario = Scenario.new("test")
	-- scenario:move(player, 0.0, 1000.0, 1000.0)
	-- scenario:wait(1000.0)
	-- scenario:move(player, 1000.0, 0.0, 1000.0)
	-- scenarioManager:add(scenario)
	-- --scenarioManager:execute("test")
	-- scenarioManager:executeAll()
	--scenarioManager:remove("test")
	--scenarioManager:removeAll()
	
	text = GUIText.new("text")	
	animation = false
	text:setColor("#FFFF00FF")
	text:setBackground("#00000099")
	textNode = Node.new("Text")
	textNode:setText(text)
	playerWidth = player:getScaleX()
	playerHeight = player:getScaleY()
	facingRight = true
	timeDelta = 0
	ratio = 1.0
	velocityX = 0
	velocityY = 0
	lastLevel = 1
	currLevel = 1
	playerPosX = 0
	posOffset = 0
	waitForButton = false
	animation = false
	moveCamera = false
	setupLevel(1)
end

-- Called when program is brought to foreground.
function resume()
	print("Called resume()")
end

-- Called when program screen resolution is changed.
function resize()
	print("Called resize()")
	ratio = getScreenHeight() / bckgHeight
	screenWidth = getScreenWidth()
	background:setScaleXY(bckgWidth * ratio, bckgHeight * ratio)
	parallax:setScaleXY(background:getScaleX() * ratio, bckgHeight * ratio)
	player:setScaleXY(playerWidth * ratio, playerHeight * ratio)
	print("PlayerWidth: " .. playerWidth .. ", ratio: " .. ratio)
end

-- Called every frame.
function update()
	timeDelta = getTimeDelta() / 16.666
	-- Handle input.
	keyPressed = input:keyPressed(constants["MOUSE_L"]) or input:keyPressed(constants["TOUCH"])
	if not animation then
		if (input:keyPressed(constants["A"]) or (input:getPointerX() < screenWidth / 4 and keyPressed)) and velocityX > -MAX_VELOCITY * ratio then
			moveLeft()
		end
		if (input:keyPressed(constants["D"]) or (input:getPointerX() > screenWidth / 4 * 3 and keyPressed)) and velocityX < MAX_VELOCITY * ratio then
			moveRight()
		end
	end
	if input:keyReleased(constants["Q"]) then
		handleButton(1)
	elseif input:keyReleased(constants["W"]) then
		handleButton(2)
	end
	updatePositions()
	updateText()
	updateAnimation()
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end