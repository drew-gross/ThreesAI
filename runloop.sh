while true
do
	git pull
	pod install
	xcodebuild -scheme "Release ThreesAI" build
	./DerivedData/ThreesAI/Build/Products/Release/ThreesAI
done
