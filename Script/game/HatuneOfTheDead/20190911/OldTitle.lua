--
-- Title
--

-- �^�C�g����ʎ���
subclass("Title", Actor) {
		__tostring = function(self)
			return("class Title name="..self.name) 
		end
}

-- �R���X�g���N�^
function Title:ctor(...)
	print("Title:ctor() called")
	Title.super.ctor(self, ...)
end

-- ������
function Title:init(cfg)
	-- �X�[�p�[�N���X(Actor)��init���Ă�
	print("Title:init");
	Actor.init(self, cfg)
	self.label = GUI.Label:new_local(0,0,200,200)
	GUI.Manager:instance():add(self.label)
end

-- Actor�폜���Ɏ��s�����
function Title:on_destroy()
	print("Title:on_destroy()");
	GUI.Manager:destory()--�ÓI�N���X�֐�
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	state:setNeedDrawCall(false);
	-- self.label:delete()
end


-- ��Ԋ֐�

function Title:state_start(rt)
	print("Title:state_start");
	while not GUI.Manager:instance():isReady() do
		self:wait(0)
	end
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	state:setNeedDrawCall(true);
	
	return "goto", "state_frame"
end

function Title:state_frame(rt)
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	
	self.label:setText("3")
	self:wait(60)
	
	print("2")
	self.label:setText("2")
	self:wait(60)
	
	print("1")
	self.label:setText("1")
	self:wait(60)
	
	print("0")
	-- self.label:setIsEnable(false)
	self.label:setFontColor(0xffff0000)
	local ok = GUI.OkDialog:new_local()
	ok:show()
	while not ok:isOk() do
		self:wait(0)
	end
	ok:hide()
	
	self.label:setText("go")
	self:wait(60)
	
	
	change_scene(Lobby)
	self:wait(0)--�Ȃ��Ɖ��̍s���s����on_destroy()��2�x�Ă΂��
	return "exit"
	
end

