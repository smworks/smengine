-- Calculate maximum square corner to fit into current window.
function getMaxSquareSide()
	width = getScreenWidth()
	height = getScreenHeight()
	if width > height then
		return height
	else
		return width
	end
end

function setupBoard(board)
	side = getMaxSquareSide()
	if side == getScreenHeight() then
		board:setPosX((getScreenWidth() - side) / 2.0)
	end
	if side == getScreenWidth() then
		board:setPosY((getScreenHeight() - side) / 2.0)
	end
	board:setScaleX(side)
	board:setScaleY(side)
end

-- Returns tuple which indexes pointing to selected cell.
function getCell(boardWidth, boardHeight)
	if input == nil then
		input = getInput()
	end
	local screenWidth = getScreenWidth()
	local screenHeight = getScreenHeight()
	local relativeX = input:getPointerX() - (screenWidth - boardWidth) / 2
	local relativeY = input:getPointerY() - (screenHeight - boardHeight) / 2
	local col = -1
	local row = -1
	local cellSize = boardWidth / 3
	if relativeY > 0 and relativeY < cellSize then
		row = 0
	elseif relativeY > cellSize and relativeY < cellSize * 2 then
		row = 1
	elseif relativeY > 2 * cellSize and relativeY < cellSize * 3 then
		row = 2
	end
	if relativeX > 0 and relativeX < cellSize then
		col = 0
	elseif relativeX > cellSize and relativeX < cellSize * 2 then
		col = 1
	elseif relativeX > 2 * cellSize and relativeX < cellSize * 3then
		col = 2
	end
	return row, col
end