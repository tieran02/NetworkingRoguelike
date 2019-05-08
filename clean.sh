rm -rf ./bin
rm -rf ./bin-int

find . -type f -name '*.workspace' -exec rm {} +
find . -type f -name '*.layout' -exec rm {} +
find . -type f -name '*.depend' -exec rm {} +
find . -type f -name '*.cbp' -exec rm {} +