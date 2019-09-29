--
-- Clear
--

-- �^�C�g����ʎ���
subclass("Clear", Actor) {
		__tostring = function(self)
			return("class Clear name="..self.name) 
		end
}

-- �R���X�g���N�^
function Clear:ctor(...)
	print("Clear:ctor() called")
	Clear.super.ctor(self, ...)
end

-- ������
function Clear:init(cfg)
	-- �X�[�p�[�N���X(Actor)��init���Ă�
	print("Clear:init");
	Actor.init(self, cfg)
	-- print(cfg.score)
	self.score = cfg.score+cfg.playerHp;
	
	local fw = GameLib.Framework:instance();
	self.label = GUI.Label:new_local(
		0,0,
		fw:width(), fw:height() );
		
	self.label:setText(
	cfg.txt
	.."�c��HP: " ..tostring(cfg.playerHp) .."\n"
	.."�|�����G�̐�: " ..tostring(cfg.numKilled) .."\n"
	.."�X�R�A: " ..tostring(self.score)
	);
		
	GUI.Manager:instance():add(self.label)
	
	
end

-- Actor�폜���Ɏ��s�����
function Clear:on_destroy()
	print("Clear:on_destroy()");
	GUI.Manager:destory()--�ÓI�N���X�֐�
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	state:setNeedDrawCall(false);
end


-- ��Ԋ֐�

function Clear:state_start(rt)
	print("Clear:state_start");
	while not GUI.Manager:instance():isReady() do
		self:wait(0)
	end
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	state:setNeedDrawCall(true);
	
	return "goto", "state_frame"
end

function Clear:state_frame(rt)
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()

	local i=0;
	while i< 600 do
		i = i+1;
		self:wait(0)
	end
	
	change_scene(Title)
	self:wait(0)--�Ȃ��Ɖ��̍s���s����on_destroy()��2�x�Ă΂��
	return "exit"
	
end

