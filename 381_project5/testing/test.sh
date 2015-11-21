# script is setup to start in directory named 'testing'
cd ..

make
mv ./proj5exe ./testing
cd testing

# setup directory for saving this test run's output
N=0
dirname="test_run$N"

while [ -d "$dirname" ]
do
   N=`expr $N + 1`
   dirname="test_run$N"
done

mkdir "$dirname"

# Run tests and save output
./proj5exe < fight_in.txt > "$dirname/fight_testout.txt"
diff "$dirname/fight_testout.txt" fight_out.txt > "$dirname/fight_diff.txt"
./proj5exe < fight_noshow_in.txt > "$dirname/fight_noshow_testout.txt"
diff "$dirname/fight_noshow_testout.txt" fight_noshow_out.txt > "$dirname/fight_noshow_diff.txt"
./proj5exe < name_validity_in.txt > "$dirname/name_validity_testout.txt"
diff "$dirname/name_validity_testout.txt" name_validity_out.txt > "$dirname/name_validity_diff.txt"
./proj5exe < states_in.txt > "$dirname/states_testout.txt"
diff "$dirname/states_testout.txt" states_out.txt > "$dirname/states_diff.txt"
./proj5exe < view_in.txt > "$dirname/view_testout.txt"
diff "$dirname/view_testout.txt" view_out.txt > "$dirname/view_diff.txt"
./proj5exe < workviolence_in.txt > "$dirname/workviolence_testout.txt"
diff "$dirname/workviolence_testout.txt" workviolence_out.txt > "$dirname/workviolence_diff.txt"
./proj5exe < workviolence_noshow_in.txt > "$dirname/workviolence_noshow_testout.txt"
diff "$dirname/workviolence_noshow_testout.txt" workviolence_noshow_out.txt > "$dirname/workviolence_noshow_diff.txt"

mv ./proj5exe "$dirname"
cd "$dirname"

# Check if all diff tests passed
diffsize=$(stat -c%s "fight_noshow_diff.txt")
diffsize=`stat -c%s "fight_diff.txt" | expr + $diffsize`
diffsize=`stat -c%s "name_validity_diff.txt" | expr + $diffsize`
diffsize=`stat -c%s "states_diff.txt" | expr + $diffsize`
diffsize=`stat -c%s "workviolence_diff.txt" | expr + $diffsize`
diffsize=`stat -c%s "workviolence_noshow_diff.txt" | expr + $diffsize`

if [ $diffsize == 0 ]; then
   echo "All diff tests passed"
else
   echo "DIFF TEST FAILED"
fi

cd ..
