while true
do
	git pull
	pod install
	xcodebuild -workspace ThreesAI.xcworkspace -scheme "Release ThreesAI"
	./DerivedData/ThreesAI/Build/Products/Release/ThreesAI
done
