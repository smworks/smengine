--
-- DESCRIPTION:  Maze generation program Lua <-> C:
--               Lua - maze generation
--               C   - maze visualization
--      AUTHOR:  Alexander Simakov, <xdr [dot] box [at] Gmail>
--               http://alexander-simakov.blogspot.com/
--     LICENSE:  Public domain
--  HOW-TO RUN:  gcc -o maze_generator -Wall `pkg-config lua5.1 --libs --cflags` maze_generator.c
--              ./maze_generator ./maze_dfs.lua 15 10
--
--               OR
--
--               Uncomment usage example at the end of the file and
--               run as a stand alone Lua program:
--               chmod 755 maze_dfs.lua
--               ./maze_dfs.lua

local Maze = {}

-- Create new Maze instance
function Maze:new(width, height, cellSize, blankTexture)
    local obj = {
        width       = width,
        height      = height,
        cells       = {},
		size		= cellSize,
		texture		= blankTexture
    }

    setmetatable(obj, self)
    self.__index = self

    return obj
end

-- Generate maze
function Maze:generate()
    self:_init_cells()

    --self.cells[1][1].left_wall                     = false -- open entry point
    self.cells[self.height][self.width].right_wall = false -- open exit point

    self:_process_neighbor_cells(1, 1);
    local result = self:_render()

    return result
end

-- Close all walls, mark all cells as not visited yet
function Maze:_init_cells()
    self.cells = {}
    for y = 1, self.height do
        self.cells[y] = {}
        for x = 1, self.width do
            self.cells[y][x] = {
                left_wall   = true,
                right_wall  = true,
                top_wall    = true,
                bottom_wall = true,
                is_visited  = false,
            }
        end
    end

    return
end

-- Get cells neighbor to the cell (x,y): left, right, top, bottom
function Maze:_get_neighbor_cells(x, y)
    local neighbor_cells = {}

    local shifts = {
        { x = -1, y =  0 },
        { x =  1, y =  0 },
        { x =  0, y =  1 },
        { x =  0, y = -1 },
    }

    for index, shift in ipairs(shifts) do
        new_x = x + shift.x
        new_y = y + shift.y

        if new_x >= 1 and new_x <= self.width  and
           new_y >= 1 and new_y <= self.height
        then
            table.insert( neighbor_cells, { x = new_x, y = new_y } )
        end
    end

    return neighbor_cells
end

-- Process the cell with all its neighbors in random order
function Maze:_process_neighbor_cells(x, y)
    self.cells[y][x].is_visited = true

    local neighbor_cells = self:_shuffle(self:_get_neighbor_cells(x, y))

    for index, neighbor_cell in ipairs(neighbor_cells) do
        if self.cells[neighbor_cell.y][neighbor_cell.x].is_visited == false then
            if neighbor_cell.x > x     then     -- open wall with right neighbor
                self.cells[y][x].right_wall                              = false
                self.cells[neighbor_cell.y][neighbor_cell.x].left_wall   = false
            elseif neighbor_cell.x < x then     -- open wall with left neighbor
                self.cells[y][x].left_wall                               = false
                self.cells[neighbor_cell.y][neighbor_cell.x].right_wall  = false
            elseif neighbor_cell.y > y then     -- open wall with bottom neighbor
                self.cells[y][x].bottom_wall                             = false
                self.cells[neighbor_cell.y][neighbor_cell.x].top_wall    = false
            elseif neighbor_cell.y < y then     -- open wall with top neighbor
                self.cells[y][x].top_wall                                = false
                self.cells[neighbor_cell.y][neighbor_cell.x].bottom_wall = false
            end

            -- recursively process this cell
            self:_process_neighbor_cells(neighbor_cell.x, neighbor_cell.y)
        end
    end

    return
end

function Maze:_render()
    for y = 1, self.height do
        for x = 1, self.width do
			row = self.height - y
			col = x - 1
            if self.cells[y][x].left_wall == true then
				startIndex = row * self.size - 1;
				if startIndex < 0 then startIndex = 0 end
				self.texture:line(startIndex, col * self.size,
					(row + 1) * self.size - 1, col * self.size,
					255, 255, 255, 255)
            end
            if self.cells[y][x].top_wall == true then
				self.texture:line((row + 1) * self.size - 1, col * self.size,
					(row + 1) * self.size - 1, (col + 1) * self.size - 1,
					255, 255, 255, 255)
            end
			if col == self.width - 1 and self.cells[y][x].right_wall == true then
				self.texture:line(row * self.size, (col + 1) * self.size - 1,
					(row + 1) * self.size - 1, (col + 1) * self.size - 1,
					255, 255, 255, 255)
            end
            if row == 0 and self.cells[y][x].bottom_wall == true then
				self.texture:line(0, col * self.size, 0, (col + 1) * self.size - 1,
					255, 255, 255, 255)
            end

        end
    end
end

-- Shuffle array (external_rand() is external C function)
function Maze:_shuffle(array)
    for i = 1, #array do
        index1 = math.random(#array)
        index2 = math.random(#array)

        if index1 ~= index2 then
            array[index1], array[index2] = array[index2], array[index1]
        end
    end

    return array
end

-- Split string into array of chars
function Maze:_string_to_array(str)
    local array = {}
    for char in string.gmatch(str, '.') do
        table.insert(array, char)
    end

    return array
end

--
-- Procedural interface
--

function generate_maze(width, height, size, texture)
    maze = Maze:new(width, height, size, texture)
    maze:generate()
end