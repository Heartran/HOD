--
-- Loop
--

subclass("Loop", Actor) {
		__tostring = function(self)
			return("class Title name="..self.name) 
		end
}

-- �R���X�g���N�^
function Loop:ctor(...)
	print("Loop:ctor() called")
	Loop.super.ctor(self, ...)
end

-- ������
function Loop:init(cfg)
	-- �X�[�p�[�N���X(Actor)��init���Ă�
	-- print("Loop:init");
	Actor.init(self, cfg)
end

-- Actor�폜���Ɏ��s�����
function Loop:on_destroy()
end


-- ��Ԋ֐�

function Loop:state_start(rt)
	-- �J�����̐ݒ�
	-- �X�e�[�W���f���̃��[�h
	local xml = GameLib.FileIO.InFile.create(cfg.stage_data[1]:modelName);
  while not xml:isReady() do
    wait(1)
	end

  print("plane.xml load ok")
  local serialize_accessory = Serialization.GameObject.Accessory:new_local();
  serialize_accessory:loadXml( xml );
  
  while not accessory:isReady() do
    wait(1)
	end
	self.accessory = accessory:instance( 
		nil,
		btTransform:getIdentity()
	)
  
  self.stage:addToWorld()
	
	-- event�̓o�^
	-- player�̍쐬
	self.player = GameObject.Player:create()
	self.player:load()
	-- enemy�̍쐬
	self.serialize_enemy = new Serialization.GameObject.Enemy;
	self.serialize_enemy:loadXml("Data/Serialization/GameObject/Enemy/bou.xml");
	while not self.serialize_enemy:isReady() do
		wait(1)
	end
	
	self.enemies = std.vector_EnemySharedPtr_:new_local()
	
	for i = 0, 10 do
		local e = mEnemy->instance(mPlayer);	
		self.enemies:push_back( e );
		e->addToWorld();
	end
	
	return "goto", "state_frame"
end

function Loop:state_frame(rt)
	return "exit"
end

