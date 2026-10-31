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
public:
	Player(int hp, string name) : Entity(hp, name) {}
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
};

class Monster : public Entity {
public:
	Monster(int hp, string name) : Entity(hp, name) {}
};

class BossMonster : public Monster {
	bool isRage = false;
public:
	BossMonster(int hp, string name) : Monster(hp, name) {}
	void attack(Entity* target, int damage) {
		if (!isRage && hp <= 100) {
			isRage = true;
			cout << "\n [경고] " << name << "이(가) 분노하여 광폭화 상태의 돌입합니다! 공격력 2배! \n\n";
		}
		Monster::attack(target, isRage ? 2 * damage : damage);
	}
};
int main(void) {
	int count = 1;
	unique_ptr<Player> player = make_unique<Player>(200, "주인공");
	unique_ptr<Monster> m1 = make_unique<Monster>(80, "고블린");
	player->addItem(make_unique<Potion>("빨간 물약", 30));
	player->addItem(make_unique<Bomb>("소형 폭탄", 30));
	while (player->isAlive() && m1->isAlive()) {
		player->attack(m1.get(), 30);
		if (!m1->isAlive())
			break;
		m1->attack(player.get(), 20);
		unique_ptr<Item> item = player->firstItem();
		if (item != nullptr) {
			if (Potion* p = dynamic_cast<Potion*>(item.get())) {
				p->use(player.get());
			}
			else if (Bomb* b = dynamic_cast<Bomb*>(item.get())) {
				b->use(m1.get());
			}
		}
		cout << "-------------------------\n";
	}
	cout << "보스 등장!!\n";
	unique_ptr<BossMonster> boss1 = make_unique<BossMonster>(200, "드래곤");
	while (player->isAlive() && boss1->isAlive()) {
		player->attack(boss1.get(), 40);
		if (!boss1->isAlive())
			break;
		boss1->attack(player.get(), 25);
		unique_ptr<Item> item = player->firstItem();
		if (item != nullptr) {
			if (Potion* p = dynamic_cast<Potion*>(item.get())) {
				p->use(player.get());
			}
			else if (Bomb* b = dynamic_cast<Bomb*>(item.get())) {
				b->use(boss1.get());
			}
		}
		cout << "-------------------------\n";
	}
	return 0;
}