#include <common.hpp>





void Combat::Init()
{

	this->f_ont = new FontManager();

	this->font = f_ont->GetFont();

	this->ftext = new FloatingText();

}



void Combat::SetPlayerTarget(Player * player, EnemyManager * enemies)
{




	player->SetTarget(NO_TARGET);


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{




		if (glm::distance(player->GetPAttributes()->position, enemies->GetEnemiesPointer()[0][i]->GetPAttributes()->position) < MIN_DISTANCE)
		{


			if (player->GetTarget() == NO_TARGET)
			player->SetTarget(i);



			if (player->GetActionHandler()->GetAction() == ATTACKING)
			{
				player->GetDirection()->Compute(DIR_TYPE_4, player->GetPAttributes()->position,
					enemies->GetEnemiesPointer()[0][i]->GetPAttributes()->target);

			}


			break;



		}


	}




}

  

void Combat::PlayerAttack(GameObject * g_obj, Player * player, EnemyManager *enemies, Map *current_map)
{

	Item *item;
	
	item = g_obj->GetItemList()->GetList()[rand()%g_obj->GetItemList()->GetList().size()];

	if (player->GetTarget() > NO_TARGET && 
		player->GetActionHandler()->GetAction() == ATTACKING && 
		player->GetActionHandler()->HasReachedLifetime(TIME_FOR_ACTION))
	{

		
		if (enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->GetHp()->hp - player->GetItems()[ITEM_SLOT_WEAPON]->attack.x <= 0)
		{

		
			
				g_obj->GetItemList()->AddDroppedItem((int)(enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetPAttributes()->position.x), (int)(enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetPAttributes()->position.y), item);
		       // std::cout << g_obj->GetItemList()->GetDroppedItems().size()<<"\n";
				enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->GetHp()->Damage(player->GetItems()[ITEM_SLOT_WEAPON]->attack);
				player->GetEventHandler()->Init(current_map, g_obj);
			
		}

		enemies->GetEnemiesPointer()[0][player->GetTarget()]->GetStats()->GetHp()->Damage(player->GetItems()[ITEM_SLOT_WEAPON]->attack);
		//ftext->Add(font, "test", player->GetPAttributes()->position, 32, 0, 5, 1);
		g_obj->GetTurnSystem()->ComputeAttack(player->GetItems()[ITEM_SLOT_WEAPON]->attack_speed);
		player->GetActionHandler()->SetAction(NO_ACTION);
		player->GetActionHandler()->Stop();


	}




}




void Combat::CheckPlayerMoveAbility(Player * player, EnemyManager * enemies)
{


	GLboolean advance = true;



	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{



		if (!enemies->GetEnemiesPointer()[0][i]->GetActionHandler()->IsStopped())
		{

			advance = false;
			break;

		}




	}


	player->SetMoveAbility(advance);



}




void Combat::PlayerRelated(GameObject * g_obj, Player * player, EnemyManager * enemies, Map * map)
{


	this->CheckPlayerMoveAbility(player, enemies);
	this->SetPlayerTarget(player, enemies);
	this->PlayerAttack(g_obj, player, enemies, map);


}




void Combat::SortThingsOut(Player * player, EnemyManager * enemies)
{


	std::vector<Enemy*>*m_enemies = enemies->GetEnemiesPointer();



	for (GLuint i = 0; i < m_enemies->size(); i++)
	{


		for (GLuint j = 0; j < m_enemies->size(); j++)
		{


			if (glm::distance(m_enemies[0][i]->GetPAttributes()->position, player->GetPAttributes()->position) <
				glm::distance(m_enemies[0][j]->GetPAttributes()->position, player->GetPAttributes()->position))
				std::swap(m_enemies[0][i], m_enemies[0][j]);


		}


	}




}



void Combat::SetEnemyTarget(Player * player, EnemyManager * enemies)
{




	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		enemies->GetEnemiesPointer()[0][i]->SetTarget(NO_TARGET);


		if (glm::distance(player->GetPAttributes()->position, enemies->GetEnemiesPointer()[0][i]->GetPAttributes()->position) < MIN_DISTANCE)
			enemies->GetEnemiesPointer()[0][i]->SetTarget(0);



	}


}




void Combat::AquireEnemyTarget(Player * player, EnemyManager * enemies)
{



	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		Enemy * current_enemy = enemies->GetEnemiesPointer()[0][i];



		if (glm::distance(player->GetPAttributes()->position, current_enemy->GetPAttributes()->position) < MIN_AQUIRE_DISTANCE)
			current_enemy->SetTargetPosition(player->GetPAttributes()->target);
		if (glm::distance(player->GetPAttributes()->position, current_enemy->GetPAttributes()->position) > 2.0f * MIN_AQUIRE_DISTANCE)
			current_enemy->SetTargetPosition(player->GetPAttributes()->target);




	}




}




void Combat::EnemyAttack(GameObject * g_obj, Player * player, EnemyManager *enemies)
{


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		Enemy * current_enemy = enemies->GetEnemiesPointer()[0][i];


		if (current_enemy->GetTarget() > NO_TARGET && current_enemy->GetActionHandler()->IsStopped())
		{


			if (current_enemy->GetTurnSystem()->GetTurns() >= 1.0f/current_enemy->GetStats()->base_attack_speed)
			{


				current_enemy->GetDirection()->Compute(DIR_TYPE_4, current_enemy->GetPAttributes()->position, current_enemy->GetTargetPosition());
				current_enemy->GetActionHandler()->SetAction(ATTACKING);
				current_enemy->GetActionHandler()->Start();


			}


		}



		if (current_enemy->GetActionHandler()->GetAction() == ATTACKING && 
			current_enemy->GetActionHandler()->HasReachedLifetime(TIME_FOR_ACTION))
		{
		

			player->GetStats()->GetHp()->Damage(current_enemy->GetStats()->base_attack);
			current_enemy->GetTurnSystem()->ComputeAttack(-current_enemy->GetStats()->base_attack_speed);
			current_enemy->GetActionHandler()->SetAction(NO_ACTION);
			current_enemy->GetActionHandler()->Stop();
		
		
		}




	}


}





void Combat::EnemyMovement(Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager * enemies)
{




	if (player->GetPAttributes()->HasReachedTarget())
		enemies->GetEnemiesPointer()[0][0]->GetTurnLogic()->Advance();



	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
	{


		Enemy * enemy = enemies->GetEnemiesPointer()[0][i];
		PhysicalAttributes * attr = enemy->GetPAttributes();
		TurnSystem * turns = enemy->GetTurnSystem();
		AutoPath * a_path = enemy->GetAutoPath();
		Stats * stats = enemy->GetStats();
		ActionHandler * a_handler = enemy->GetActionHandler();



		if (!enemy->GetTurnLogic()->IsUseless())
		{

			if (attr->position == attr->target &&
				enemy->GetTargetPosition() != vec2_0 &&
				turns->GetTurns() >= 1.0f / stats->base_movement_speed
				&& a_handler->IsStopped())
			{




				a_path->Start(g_obj, enemy->GetPAttributes()->position, enemy->GetTargetPosition());
				a_path->Reset();




				if (a_path->GetPathfinder()->GetPathFound())
				{


					a_path->SetPath(a_path->GetPathfinder()->GetPath());
					a_path->Advance();




				}
				else
				{
					enemy->GetTurnLogic()->Reset();
					turns->Reset();

				}

			}



			if (a_path->IsSet() && !a_path->FinishedWithoutLast() && turns->GetTurns() >= 1.0f / stats->base_movement_speed)
			{



				attr->target = a_path->GetStep();
				g_obj->GetCollisionMap()->AddToList(glm::ivec2(attr->target));



				if (a_handler->IsStopped())
				{
					a_handler->Start();
					a_handler->SetAction(MOVING);
				}
	

				if (a_handler->HasReachedLifetime(TIME_FOR_ACTION))
				{
			
					turns->ComputeMovement(-stats->base_movement_speed);
					enemy->GetTurnLogic()->Advance();

					a_handler->Stop();
					a_handler->SetAction(NO_ACTION);
				}



			}


		}
	}


}



void Combat::UpdateTurns(GameObject * g_obj, EnemyManager * enemies)
{


	for (GLuint i = 0; i < enemies->GetEnemiesPointer()->size(); i++)
		if (enemies->GetEnemiesPointer()[0][i]->GetTargetPosition() != vec2_0)
			g_obj->GetTurnSystem()->Update(enemies->GetEnemiesPointer()[0][i]->GetTurnSystem());

}





void Combat::EnemyRelated(Controller * ctrl, GameObject * g_obj, Player * player, EnemyManager * enemies, Map * map)
{



	this->UpdateTurns(g_obj, enemies);
	this->SortThingsOut(player, enemies);
	this->SetEnemyTarget(player, enemies);
	this->AquireEnemyTarget(player, enemies);
	this->EnemyAttack(g_obj, player, enemies);
	this->EnemyMovement(ctrl, g_obj, player, enemies);


}




void Combat::Action(Controller * ctrl,GameObject * g_obj, Player * player, EnemyManager * enemies, Map * map)
{


	
	this->PlayerRelated(g_obj, player, enemies, map);
	this->EnemyRelated(ctrl,g_obj, player, enemies, map);



	g_obj->GetTurnSystem()->Reset();


}






