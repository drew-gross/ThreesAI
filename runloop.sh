while true
do
	git pull
	xcodebuild -scheme "Release ThreesAI" build

	start=$(date +%s)
	result=$(./DerivedData/ThreesAI/Build/Products/Release/ThreesAI)
	end=$(date +%s)

	curl -X POST \
	-H "Content-Type: application/json" \
	-H "X-Parse-Application-Id: U9Q2piuJY51XQUjQ6MMFnTM3zWLopcTGQEUgiYd8" \
	-H "X-Parse-REST-API-Key: yQvZwKW7Llrb9VwQSRQAQOPUYR5LI6YFcC5eJeAp" \
	-d "{\"Score\":\"$result\",\"GitHash\":\"$(git rev-parse HEAD)\",\"TimeTaken\":\"$(($end-$start))\"}" \
	https://api.parse.com/1/classes/Data
done
