-- Constants.
MAX_VELOCITY = 15.0
CAMERA_VELOCITY = 5.0
BASE_HEIGHT = 300
NPC_TALK_RADIUS = 200

function handleButton(button)
	if currLevel == 2 then
		if button == 1 then
			print("Throw rock")
		elseif button == 2 then
			print("Call out")
		end
		text:setText("We are sorry, but the game stops here.")
		text:setFontSize(40)
		waitForButton = false
	end
end

function updateText()
	if npc == nil then
		return
	end
	local distance = npc:getPosX() - player:getPosX()
	if distance > -NPC_TALK_RADIUS and distance < NPC_TALK_RADIUS then
		text:setVisibility(true)
	else
		text:setVisibility(false)
		return
	end
end

-- Setup level.
function setupLevel(level)
	print("Setting up level: " .. level)
	currLevel = level
	if npc ~= nil then
		npc:setVisibility(false)
	end
	if level == 1 then
		text:setVisibility(false)
		portalRight = 2
		portalLeft = 0
		scene:setBackground(getNode("background"))
		scene:setBackground(getNode("parallax"), 2, 0.5)
		npc = nil
		leftOffset = 0
		resize()
		if lastLevel == 2 then
			player:setPosX(scene:getBackgroundWidth() - player:getScaleX() / 2);
			player:setScaleX(-player:getScaleX())
		else
			player:setPosX(player:getScaleX());
		end
		text:setText("")
	elseif level == 2 then
		text:setVisibility(true)
		portalRight = 0
		portalLeft = 1
		getNode("background"):setVisibility(false)
		getNode("cave"):setVisibility(true)
		scene:setBackground(getNode("cave"))
		scene:setBackground(nil, 2)
		leftOffset = 0
		resize()
		npc = getNode("npc")
		npc:setScaleX(700 * ratio)
		npc:setScaleY(700 * ratio)
		npc:setPosX(1000)
		npc:setPosY(BASE_HEIGHT * ratio)
		player:setPosX(player:getScaleX());
		text:setText("Press Q to throw a rock at him.\nPress W to come closer.")
		text:setFontSize(12)
		scenario = Scenario.new("test")
		scenario:move(player, 0.0, player:getPosY(), scene:getBackgroundWidth(), player:getPosY(), 2000.0)
		scenarioManager:execute(scenario)
	end
	if npc ~= nil then
		npc:setVisibility(true)
	end
	player:setPosY(BASE_HEIGHT * ratio)
	velocityX = 0
	lastLevel = level
	updatePositions()
end

function moveLeft()
	if facingRight then
		facingRight = false
		player:setScaleX(-player:getScaleX())
		posOffset = player:getScaleX()
	end
	velocityX = velocityX - timeDelta
end

function moveRight()
	if not facingRight then
		facingRight = true
		player:setScaleX(-player:getScaleX())
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
	player:addPosX(velocityX)
	if player:getPosX() > scene:getBackgroundWidth() - player:getScaleX() / 2 then
		player:setPosX(scene:getBackgroundWidth()  - player:getScaleX() / 2)
		velocityX = 0
		if portalRight > 0 then
			setupLevel(portalRight)
			return
		end
	elseif player:getPosX() < leftOffset + player:getScaleX() / 2 then
		player:setPosX(leftOffset + player:getScaleX() / 2)
		velocityX = 0
		if portalLeft > 0 then
			setupLevel(portalLeft)
			return
		end
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
	playSound("sound.mid", true)
	input = getInput()
	player = getNode("player")
	scenarioManager = getScenarioManager()
	scene = getSceneManager()
	scene:setSceneHeight(1000)
	scene:setTarget(player)
	text = GUIText.new("text")	
	text:setColor("#FFFF00FF")
	text:setBackground("#00000099")
	screenWidth = getScreenWidth()
	facingRight = true
	timeDelta = 0
	ratio = 1.0
	velocityX = 0
	velocityY = 0
	lastLevel = 1
	currLevel = 1
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
end

-- Called every frame.
function update()
	if input:keyPressed(constants["ESC"]) then
		exit()
		return
	end
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
	if input:keyPressed(constants["G"]) then
		player:setPosX(100.0)
		if player:getVisibility() then
			print("PLAYER VISIBLE")
		end
		print("Player scale: " .. player:getScaleX() .. ", " .. player:getScaleY())
		print("Player pos x: " .. player:getPosX())
		print("Player pos y: " .. player:getPosY())
		print("Player pos z: " .. player:getPosZ())
		print("Camera pos x: " .. getCamera():getPosX())
		print("Camera pos y: " .. getCamera():getPosY())
	end
	if input:keyPressed(constants["H"]) then
		par = getNode("cave")
		print("Pos: " .. par:getPosX() .. ", " .. par:getPosY())
		print("Z: " .. par:getPosZ() .. ", " .. par:getScaleZ())
		print("Scale: " .. par:getScaleX() .. ", " ..  par:getScaleY())
	end
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end