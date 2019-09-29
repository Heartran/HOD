

-- �Q�̋�`���Փ˂��邩�ǂ�����Ԃ�
local function is_intersect_rects(x1,y1,w1,h1, x2,y2,w2,h2)
	local x_center1, y_center1, x_size1, y_size1 = x1,y1,w1,h1
	local x_center2, y_center2, x_size2, y_size2 = x2,y2,w2,h2
	local x_size1_half = x_size1 * 0.5
	local y_size1_half = y_size1 * 0.5
	local x_size2_half = x_size2 * 0.5
	local y_size2_half = y_size2 * 0.5
	local x1L = x_center1 - x_size1_half
	local x1R = x_center1 + x_size1_half
	local y1U = y_center1 - y_size1_half
	local y1D = y_center1 + y_size1_half
	local x2L = x_center2 - x_size2_half
	local x2R = x_center2 + x_size2_half
	local y2U = y_center2 - y_size2_half
	local y2D = y_center2 + y_size2_half
	if (x1R < x2L) or (x2R < x1L) or (y1D < y2U) or (y2D < y1U) then return false end
	return true
end


-- �Փ˔���N���X
class ("CollisionDetector") {
}

-- �R���X�g���N�^
function CollisionDetector:ctor(...)
    self.grid = {}
    self.x_gridspan = 100
    self.y_gridspan = 100
    self.x_fieldsize = 640*4
    self.y_fieldsize = 480
    self.x_gridcount = 0
    self.y_gridcount = 0
    self.total_gridcount = 0
    
    self.results = {}
end

local floor = math.floor

function CollisionDetector:init(fieldsize_x, fieldsize_y, gridspan_x, gridspan_y)
	self.x_gridspan = gridspan_x
    self.y_gridspan = gridspan_y
    self.x_fieldsize = fieldsize_x
    self.y_fieldsize = fieldsize_y
	
    self.x_gridcount = floor((self.x_fieldsize - 1) / self.x_gridspan) + 1
    self.y_gridcount = floor((self.y_fieldsize - 1) / self.y_gridspan) + 1
    self.total_gridcount = self.x_gridcount * self.y_gridcount
    
    -- �e�[�u�����u�z��v�Ƃ��Ĉ�����悤�ɍאS�̒��ӂ𕥂�
	self.grid = {}
    for i= 1, self.total_gridcount do
        self.grid[i] = {}
    end
end

-- �I�u�W�F�N�g�i�A�N�^�[�j���Փ˔���O���b�h�ɒǉ�
function CollisionDetector:add_object(actor, x1,y1,w1,h1)
	local x_center, y_center, x_size, y_size = x1,y1,w1,h1
	local x_size_half = x_size * 0.5
	local y_size_half = y_size * 0.5
	local cx1 = floor((x_center - x_size_half) / self.x_gridspan)
	local cy1 = floor((y_center - y_size_half) / self.y_gridspan)
	local cx2 = floor((x_center + x_size_half) / self.x_gridspan)
	local cy2 = floor((y_center + y_size_half) / self.y_gridspan)
	
	-- �I�u�W�F�N�g�͈̔͂������邷�ׂẴZ���ɃA�N�^�[��ǉ�
	for x = cx1,cx2 do
		for y = cy1,cy2 do
			local cellpos = y * self.x_gridcount + x + 1
			if cellpos >= 1 and cellpos <= self.total_gridcount then
				local cell = self.grid[cellpos]
				cell[#cell+1] = actor
			end
		end
	end
end

-- �Փ˂���I�u�W�F�N�g�����W
function CollisionDetector:search_collided_objects(x1,y1,w1,h1)
	return self:search_near_objects(x1,y1,w1,h1, true)
end

-- �߂��ɂ���I�u�W�F�N�g�����W
-- ���������J�E���g��Ԃ��B���ʂ�get_result�œ���
function CollisionDetector:search_near_objects(x1,y1,w1,h1, check_collision)
	local x_center, y_center, x_size, y_size = x1,y1,w1,h1
	local x_size_half = x_size/2
	local y_size_half = y_size/2
	local cx1 = floor((x_center - x_size_half) / self.x_gridspan)
	local cy1 = floor((y_center - y_size_half) / self.y_gridspan)
	local cx2 = floor((x_center + x_size_half) / self.x_gridspan)
	local cy2 = floor((y_center + y_size_half) / self.y_gridspan)
	local results = self.results
	local unique_check = {} -- ����̂��̂𕡐��ǉ����Ȃ����߂̃`�F�b�N�e�[�u��
	
	clear_table(results)

	-- �T���͈͂������邷�ׂẴZ������A�N�^�[�������W
	for x = cx1,cx2 do
		for y = cy1,cy2 do
			local cellpos = y * self.x_gridcount + x + 1
			if cellpos >= 1 and cellpos <= self.total_gridcount then
				for i,v in ipairs(self.grid[cellpos]) do
					if unique_check[v] ~= true then
						-- ���m�ȏՓ˔��������ꍇ�͍s���B�����łȂ����
						-- ��G�c�Ȕ���ŕԂ�
						if not check_collision 
							or is_intersect_rects(x1,y1,w1,h1, v:get_collision_rect()) then
							results[#results+1] = v
							unique_check[v] = true
						end
					end
				end
			end
		end
	end
	return #results
end

-- �O���b�h���̃f�[�^�����ׂċ�ɂ��āAinit��̏�Ԃ܂Ŗ߂��B
function CollisionDetector:clear()
    -- �S�O���b�h�Z�����̃I�u�W�F�N�g���X�g����ɂ���
    for i= 1, self.total_gridcount do
        local t = self.grid[i]
        for i,v in ipairs(t) do
            t[i] = nil
        end
    end
end

-- ���ʃA�N�^�[��Ԃ�
-- index��0�x�[�X�Ƃ���
function CollisionDetector:get_result(index)
    return self.results[index+1]
end


