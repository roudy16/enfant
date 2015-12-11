CC = g++
LD = g++
CFLAGS = -c -g -std=c++14 -pedantic-errors -Wall
LFLAGS = -g

OBJS = p6_main.o Model.o View.o Controller.o 
OBJS += Map.o Status.o World_map.o Local_map.o Health_status.o Amount_status.o
OBJS += Sim_object.o Structure.o Moving_object.o Agent.o
OBJS += Farm.o Town_Hall.o
OBJS += Peasant.o Infantry.o Soldier.o Archer.o Mage.o
OBJS += Agent_factory.o Structure_factory.o
OBJS += Geometry.o Utility.o
PROG = p6exe

default: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

p6_main.o: p6_main.cpp Controller.h
	$(CC) $(CFLAGS) p6_main.cpp

Model.o: Model.cpp Model.h View.h Sim_object.h Structure.h Agent.h Agent_factory.h Structure_factory.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Model.cpp

View.o: View.cpp View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) View.cpp

Map.o: Map.cpp Map.h Geometry.h View.h Utility.h
	$(CC) $(CFLAGS) Map.cpp

Status.o: Status.cpp Status.h Geometry.h View.h Utility.h
	$(CC) $(CFLAGS) Status.cpp

World_map.o: World_map.cpp World_map.h Geometry.h View.h Utility.h
	$(CC) $(CFLAGS) World_map.cpp

Local_map.o: Local_map.cpp Local_map.h Geometry.h View.h Utility.h
	$(CC) $(CFLAGS) Local_map.cpp

Health_status.o: Health_status.cpp Health_status.h Geometry.h View.h Utility.h
	$(CC) $(CFLAGS) Health_status.cpp

Amount_status.o: Amount_status.cpp Amount_status.h Geometry.h View.h Utility.h
	$(CC) $(CFLAGS) Amount_status.cpp

Controller.o: Controller.cpp Controller.h Model.h View.h Sim_object.h Structure.h Agent.h Agent_factory.h Structure_factory.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Controller.cpp

Sim_object.o: Sim_object.cpp Sim_object.h Geometry.h
	$(CC) $(CFLAGS) Sim_object.cpp

Structure.o: Structure.cpp Structure.h Model.h Sim_object.h Geometry.h
	$(CC) $(CFLAGS) Structure.cpp

Farm.o: Farm.cpp Farm.h Structure.h Sim_object.h Geometry.h
	$(CC) $(CFLAGS) Farm.cpp

Town_Hall.o: Town_Hall.cpp Town_Hall.h Structure.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Town_Hall.cpp

Agent.o: Agent.cpp Agent.h Model.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Agent.cpp

Peasant.o: Peasant.cpp Peasant.h Agent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Peasant.cpp

Infantry.o: Infantry.cpp Infantry.h Agent.h Utility.h
	$(CC) $(CFLAGS) Infantry.cpp

Soldier.o: Soldier.cpp Soldier.h Infantry.h Agent.h Utility.h
	$(CC) $(CFLAGS) Soldier.cpp

Archer.o: Archer.cpp Archer.h Infantry.h Agent.h Utility.h Model.h
	$(CC) $(CFLAGS) Archer.cpp

Mage.o: Mage.cpp Mage.h Infantry.h Agent.h Utility.h Geometry.h Model.h
	$(CC) $(CFLAGS) Mage.cpp

Moving_object.o: Moving_object.cpp Moving_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Moving_object.cpp

Agent_factory.o: Agent_factory.cpp Agent_factory.h Peasant.h Archer.h Soldier.h Mage.h Agent.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Agent_factory.cpp

Structure_factory.o: Structure_factory.cpp Structure_factory.h Farm.h Town_Hall.h Structure.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Structure_factory.cpp

Geometry.o: Geometry.cpp Geometry.h Utility.h
	$(CC) $(CFLAGS) Geometry.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(CFLAGS) Utility.cpp

clean:
	rm -f *.o
real_clean:
	rm -f $(PROG)
	rm -f *.o
