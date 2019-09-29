
-- ���[�J���ϐ��ɂ���ƃO���[�o���e�[�u���̌������Ȃ��Ȃ菭������������
local tremove = table.remove
local rawset = rawset
local tinsert = table.insert


global "orig_print"
if not _G.orig_print then
    orig_print = print
    if rawget(_G, "decoda_output") then
        print = function(...)
            local t = {}
            for i,v in ipairs({...}) do
                tinsert(t, tostring(v))
            end
            decoda_output(table.concat(t))
        end
    end
end


-- �e�[�u���̓��e��\��
local nest_level_limit = 6

function get_table_string(t, str_buf)
	return table.concat(get_table_string_sub(str_buf or {}, t, 0, {}))
end
function get_value_string(v, str_buf)
	return table.concat(get_value_string_sub(str_buf or {}, v, 0, {}))
end

-- s: ������o�b�t�@
-- t: �Ώۃe�[�u��
-- level: �l�X�e�B���O���x��(nil��)
-- stack: ��ʃX�^�b�N(nil��)
function get_table_string_sub(s, t, level, stack)
	local level = level or 0
	local stack = stack or {}
	local indent = string.rep("  ", level)
	if level > nest_level_limit then
        tinsert(s, indent)
        tinsert(s, "table : nest too deep") --
        tinsert(s, tostring(t))
	    return s
	end
	for i,v in ipairs(stack) do
		if v == t then
			tinsert(s, indent)
			tinsert(s, "table : << same as parent >> : ")
			tinsert(s, tostring(t))
			return s
		end
	end
	tinsert(stack, t)
	tinsert(s, indent)
	tinsert(s, "{ (= ")
	tinsert(s, tostring(t))
	tinsert(s, ")\n")
	for k,v in pairs(t) do
		tinsert(s, indent)
		tinsert(s, "  [")
		get_value_string_sub(s, k, level, stack)
		tinsert(s, "] = ")
		get_value_string_sub(s, v, level, stack)
		tinsert(s, "\n")
	end
    if getmetatable(t) ~= nil then
	  tinsert(s, indent)
      tinsert(s, "  <metatable> ")
      get_table_string_sub(s, getmetatable(t), level+1, stack)
	  tinsert(s, indent)
      tinsert(s, "  </metatable>\n")
    end
	tinsert(s, indent)
	tinsert(s, "}\n")
	tremove(stack)

	return s
end

-- �l���當������쐬
function get_value_string_sub(s, v, level, stack)
	local level = level or 0
	local typ = type(v)
	if typ == "table" then
		get_table_string_sub(s, v, level+1, stack)
	elseif typ == "string" then
		tinsert(s, '"')
		tinsert(s, v)
		tinsert(s, '"')
	else
		tinsert(s, typ)
		tinsert(s, ":")
		tinsert(s, tostring(v))
	end
	return s
end

-- �z��^�̃e�[�u��(t)���Ɏw��l(value)������΂�����폜���ċl�߂�B
-- �l�����������Ό��̃C���f�b�N�X��Ԃ��B
-- �l���Ȃ����nil��Ԃ��B
function remove_value_from_array(t, value)
    for i,v in ipairs(t) do
        if v == value then
            tremove(t,i)
            return i
        end
    end
	return nil
end


-- �e�[�u������l�����ׂč폜����
function clear_table(t)
	-- �z��v�f���폜
	while tremove(t) do end

	-- ����ȊO�̗v�f���폜
	for k,v in pairs(t) do
		rawset(t, k, nil)
	end
end



-- �e�[�u�����N���[��
-- �����N���z����ꍇ�̓X�^�b�N�I�[�o�[�t���[�ɂȂ�܂�

--[[ �z�Ɏア�o�[�W����
function clonetable(newtable, t)
	for k,v in pairs(t) do
		if type(v) == "table" then
			newtable[k] = clonetable({}, v)
		else
			newtable[k] = v
		end
	end
	return newtable
end
]]

-- �e�[�u�����N���[��
-- �����N���z����ꍇ��nil��Ԃ��܂�
function clonetable(newtable, t, nestcheck)
	-- �z�`�F�b�N(cycle)
    local nestcheck = nestcheck or {}
    for i,v in ipairs(nestcheck) do
        if v == t then
            --error("detected nesting in clonetable()")
            print("WARNING: detected nesting in clonetable()")
			return nil
        end
    end
	
	-- �e�[�u���̗v�f�����ꂼ��N���[��
    tinsert(nestcheck, t) -- �e�[�u�����񒆂̓l�X�g����ǉ����Ă���
	for k,v in pairs(t) do
		if type(v) == "table" then
			if is_class_instance(v) then
				-- �N���X��������V�����C���X�^���X�������clone����
				-- clone�֐����Ȃ���΃G���[�ɂȂ�
				local class = get_class(v)
				newtable[k] = class:new(v)
			else
				-- �e�[�u���Ȃ炳��ɃN���[��
				newtable[k] = clonetable({}, v, nestcheck)
			end
		else
			newtable[k] = v
		end
	end
    tremove(nestcheck)

	return newtable
end
