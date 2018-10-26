for file in "$@"
do
    cat $file | sed -e 's/#include ".\+"//'
done
