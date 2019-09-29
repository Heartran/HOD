
local reload_type = ... -- �������󂯎��


local files = {}               -- �����[�h�ΏۃX�N���v�g�t�@�C��
local reload_funcs = {}        -- �����[�h��Ɏ��s����֐�
local reload_funcs_set = {}    -- �����[�h��Ɏ��s����֐��̈�v�`�F�b�N�p

local is_reload = true -- �����[�h���A�ʏ탍�[�h���B

if reload_type == "luaTest/load" then
	-- �ʏ탍�[�h�������ŏ�������
	files = {
		"Script/common/common.lua",
		"Script/common/class.lua",
		"Script/common/coroutine.lua",
		"Script/common/collision.lua",
		"Script/common/actor.lua",
		"Script/game/luaTest/gameinit.lua",
		"Script/game/luaTest/gametitle.lua",
		"Script/game/luaTest/gametitle2.lua",
		"Script/game/luaTest/cutscene.lua",
	}
	is_reload = false
-- elseif reload_type == "chars" then
-- 	-- �L�����N�^�[�����[�h��
-- 	files = {
-- 	}
elseif reload_type == "luaTest/all" then
	-- �S�����[�h��
    local res, err = pcall( function()
        endfunc()
    end)
    print("error on endfunc() : ", err)
	files = {
		"Script/common/common.lua",
		"Script/common/class.lua",
		"Script/common/coroutine.lua",
		"Script/common/collision.lua",
		"Script/common/actor.lua",
		"Script/game/luaTest/gameinit.lua",
		"Script/game/luaTest/gametitle.lua",
		"Script/game/luaTest/gametitle2.lua",
		"Script/game/luaTest/cutscene.lua",
	}

	-- �^�C�g����ʂɖ߂�
	table.insert(reload_funcs, function() 
		startfunc()
		return true
	end)
elseif reload_type == "HatuneOfTheDead/load" then
	-- �ʏ탍�[�h�������ŏ�������
	files = {
		"Script/common/common.lua",
		"Script/common/class.lua",
		"Script/common/coroutine.lua",
		"Script/common/collision.lua",
		"Script/common/actor.lua",
		"Script/game/HatuneOfTheDead/gameinit.lua",
		"Script/game/HatuneOfTheDead/stage.lua",
		"Script/game/HatuneOfTheDead/Title.lua",
		"Script/game/HatuneOfTheDead/clear.lua",
		"Script/game/HatuneOfTheDead/GameEngineIntroduction.lua",
	}
	is_reload = false

	
elseif reload_type == "HatuneOfTheDead/all" then
	-- �S�����[�h��
	local res, err = pcall( function()
		endfunc()
	end)
	
	print("error on endfunc() : ", err)
	files = {
		"Script/common/common.lua",
		"Script/common/class.lua",
		"Script/common/coroutine.lua",
		"Script/common/collision.lua",
		"Script/common/actor.lua",
		"Script/game/HatuneOfTheDead/gameinit.lua",
		"Script/game/HatuneOfTheDead/stage.lua",
		"Script/game/HatuneOfTheDead/Title.lua",
		"Script/game/HatuneOfTheDead/clear.lua",
		"Script/game/HatuneOfTheDead/GameEngineIntroduction.lua",
	}
	
	table.insert(reload_funcs, function() 
		startfunc()
		return true
	end)

	
else
	return false, "unknown reload type:"..reload_type
end

-- �����on_reload�O���[�o���֐����폜���Ă���
if rawget(_G, "on_reload") then
    _G.on_reload = nil
end

-- �t�@�C�������[�h����
for i,path in ipairs(files) do
	local res = cfunc.DoLuaFile(path)
	local err = cfunc.GetLuaError()
	if not res then
		return false, "script<"..path.."> reload failed : "..(err or "unknown")
	end
	-- �����[�h�㏈���֐����V������`����Ă����烊�X�g�ɉ�����
	if rawget(_G,"on_reload") and (not reload_funcs_set[_G.on_reload]) then
		table.insert(reload_funcs, on_reload)
		reload_funcs_set[_G.on_reload] = path
	end
	print("script<"..path.."> reload success.")
end

if is_reload then
	-- �����[�h�㏈���֐������ׂĎ��s
	-- �P�ł�false��Ԃ����烊���[�h�͕s�����Ƃ���
	for i, func in ipairs(reload_funcs) do
		local res, err = func()
		if res == false then
			return false, "on_reload() of script<"..reload_funcs_set[func].."> failed. :"..err
		end
	end

	-- ���ׂẴN���X�̂��ׂẴC���X�^���X��on_reload()���i����΁j���s����
	exec_class_reload_funcs()

	print("reload/load success!")
end

collectgarbage("collect")

return true, "success"
