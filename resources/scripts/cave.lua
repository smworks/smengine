-- Constants.
MAX_VELOCITY = 7.0
BASE_HEIGHT = 60
JUMP_HEIGHT = 15

-- Update character and background movement.
function updateCharacter()
	-- Handle input.
	keyPressed = input:keyPressed(constants["MOUSE_L"])
	if (input:keyPressed(constants["A"]) or (input:getPointerX() < screenWidth / 4 and keyPressed)) and velocityX > -MAX_VELOCITY then
		velocityX = velocityX - timeDelta * ratio
	end
	if (input:keyPressed(constants["D"]) or (input:getPointerX() > screenWidth / 4 * 3 and keyPressed)) and velocityX < MAX_VELOCITY then
		velocityX = velocityX + timeDelta * ratio
	end
	if input:keyPressed(constants["SPACE"]) and BASE_HEIGHT == playerPosY then
		velocityY = JUMP_HEIGHT * timeDelta * ratio
	end
	-- Update positions.
	if playerPosY > BASE_HEIGHT or velocityY > 0 then
		velocityY = velocityY - timeDelta * ratio * 0.5
		playerPosY = playerPosY + velocityY
		if playerPosY < BASE_HEIGHT then
			playerPosY = BASE_HEIGHT
		end
	end
	if velocityX > 0 then
		velocityX = velocityX - timeDelta * ratio * 0.5
		if velocityX < 0 then
			velocityX = 0
		end
	else
		velocityX = velocityX + timeDelta * ratio * 0.5
		if velocityX > 0 then
			velocityX = 0
		end
	end
	playerPosX = playerPosX + velocityX
	if playerPosX > bckgWidth - playerWidth / 2 then
		playerPosX = bckgWidth - playerWidth / 2
		velocityX = 0
	elseif playerPosX < playerWidth / 2 then
		playerPosX = playerWidth / 2
		velocityX = 0
	end
	if playerPosX < halfScreen then
		background:setPosX(0)
		player:setPosX(playerPosX - playerWidth / 2)
	elseif playerPosX > bckgWidth - halfScreen then
		background:setPosX(-bckgWidth + halfScreen * 2)
		player:setPosX(playerPosX - bckgWidth + halfScreen * 2 - playerWidth / 2)
	else
		background:setPosX(-playerPosX + halfScreen)
		player:setPosX(halfScreen - playerWidth / 2)
	end
	player:setPosY(playerPosY)
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
	camera = getCamera()
	input = getInput()
	player = getNode("player")
	background = getNode("cave")
	resize()
	playerPosX = 10.0 * ratio
	playerPosY = BASE_HEIGHT
	velocityX = 0
	velocityY = 0
end

-- Called when program is brought to foreground.
function resume()
	print("Called resume()")
end

-- Called when program screen resolution is changed.
function resize()
	print("Called resize()")
	local width = background:getScaleX()
	local height = background:getScaleY()
	ratio = getScreenHeight() / height
	screenWidth = getScreenWidth()
	halfScreen = screenWidth / 2
	background:setScaleXY(width * ratio, height * ratio)
	bckgWidth = background:getScaleX()
	playerWidth = player:getScaleX()
end

-- Called every frame.
function update()
	timeDelta = getTimeDelta() / 16.666
	updateCharacter()
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end