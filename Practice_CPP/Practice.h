#include <iostream>
#include <string>
#include <memory>
#include <vector>
using namespace std;

class Entity {
protected:
	int hp;
	string name;
public:
	Entity(int hp, string name) : hp(hp), name(name) {
		cout << "HP: " << hp << " | 이름: " << name << " 생성\n";
	}
	virtual ~Entity() {
		cout << name << " 소멸\n";
	}
	void takeDamage(int damage) {
		hp -= damage;
		cout << name << " | " << damage << " 피해를 입음 | 남은 체력 : " << hp << "\n\n";
		if (hp <= 0) {
			cout << name << " 사망\n";
		}
	}
	void getHealth(int heal) {
		hp += heal;
		cout << name << " | " << heal << " 만큼 체력을 회복! | 남은 체력 : " << hp << "\n\n";
	}
	string getName() { return name; }
	void virtual attack(Entity* target, int damage) {
		cout << name << "이 " << target->name << "에게 " << damage << "만큼의 공격!\n";
		target->takeDamage(damage);
	}
	bool isAlive() const { return hp > 0; }

};
struct Item {
	string name;
	Item(string name) : name(name) {}
	virtual void use(Entity* target) = 0;
	virtual ~Item() = default;
};
struct Potion : Item {
	int healing;
	Potion(string name, int healing) : Item(name), healing(healing) {}
	void use(Entity* target) override {
		cout << "포션(" << name << ") 사용!\n";
		target->getHealth(healing);
	}
};
struct Bomb : Item {
	int damage;
	Bomb(string name, int damage) : Item(name), damage(damage) {}
	void use(Entity* target) override {
		cout << "폭탄(" << name << ") 사용!\n";
		target->takeDamage(damage);
	}
};
class Player : public Entity {
	vector<unique_ptr<Item>> inventory;
	int mp;
public:
	Player(int hp, int mp, string name) : Entity(hp, name), mp(mp) {}
	void addItem(unique_ptr<Item> item) {
		inventory.push_back(move(item));
	}
	void useItem(Entity* target) {
		if (inventory.empty())
			return;
		inventory.front()->use(target);
		inventory.erase(inventory.begin());
	}
	unique_ptr<Item> firstItem() {
		if (inventory.empty()) return nullptr;
		unique_ptr<Item> item = move(inventory.front());
		inventory.erase(inventory.begin());
		return item;
	}
	bool canUseItem() { return !inventory.empty(); }
	bool canUseSkill() { return mp >= 50; }
	void useSkill(Entity* target) {
		mp -= 50;
		attack(target, 50);
	}
};

class Monster : public Entity {
public:
	Monster(int hp, string name) : Entity(hp, name) {}
};

class BossMonster : public Monster {
	bool isRage = false;
public:
	BossMonster(int hp, string name) : Monster(hp, name) {}
	void attack(Entity* target, int damage) override {
		if (!isRage && hp <= 100) {
			isRage = true;
			cout << "\n [경고] " << name << "이(가) 분노하여 광폭화 상태의 돌입합니다! 공격력 2배! \n\n";
		}
		Monster::attack(target, isRage ? 2 * damage : damage);
	}
};
class BattleManager {
public:
	void startBattle(Player* player, Monster* monster) {
		int select = 0;
		cout << monster->getName() + "와 전투 개시!\n";
		while (player->isAlive() && monster->isAlive()) {
			while (true) {
				cout << "행동을 고르시오\n";
				cout << "(1) 공격하기\t(2) 아이템 사용";
				if (!player->canUseItem())
					cout << "(사용 불가능)";
				cout << "(3) 스킬 사용\n";
				if (!player->canUseSkill())
					cout << "(사용 불가능)";
				cin >> select;
				if (select == 1) {
					player->attack(monster, 30);
					break;
				}
				else if (select == 2 && player->canUseItem()) {
					unique_ptr<Item> item = player->firstItem();
					if (Potion* p = dynamic_cast<Potion*>(item.get())) {
						p->use(player);
					}
					else if (Bomb* b = dynamic_cast<Bomb*>(item.get())) {
						b->use(monster);
					}
					break;
				}
				else if (select == 3 && player->canUseSkill()) {
					player->useSkill(monster);
					break;
				}
			}
			if (!monster->isAlive())
				break;
			monster->attack(player, 20);
			cout << "-------------------------\n";

		}
	}
};

int main(void) {
	int count = 1;
	BattleManager battleManager;

	unique_ptr<Player> player = make_unique<Player>(200, 100, "주인공");
	unique_ptr<Monster> m1 = make_unique<Monster>(80, "고블린");

	player->addItem(make_unique<Potion>("빨간 물약", 30));
	player->addItem(make_unique<Bomb>("소형 폭탄", 30));
	battleManager.startBattle(player.get(), m1.get());
	cout << "보스 등장!!\n";
	unique_ptr<BossMonster> boss1 = make_unique<BossMonster>(200, "드래곤");

	battleManager.startBattle(player.get(), boss1.get());
	return 0;
}