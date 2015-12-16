while true
do
	git pull
	xcodebuild -scheme "Release ThreesAI" build
	./DerivedData/ThreesAI/Build/Products/Release/ThreesAI
done
