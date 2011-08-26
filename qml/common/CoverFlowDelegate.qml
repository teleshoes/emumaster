import QtQuick 1.1
import com.nokia.meego 1.0

Item {
	id: delegateItem
	x: 0
	z: PathView.z
	width: 256
	height: 256
	scale: PathView.iconScale

	property real pathAngle: PathView.angle
	property alias imageSource: dlgImg.source

	Column  {
		id: delegate
		y: coverFlow.topMargin
		spacing: 5

		Rectangle {
			id: delegateImage

			width: delegateItem.width - 8
			height: delegateItem.height - 8
			color: (dlgImg.status === Image.Ready) ? "white" : "transparent"

			z: reflection.z + 1

			Image {
				id: dlgImg

				width: delegateImage.width - 8
				height: delegateImage.height - 8
				anchors.centerIn: parent
				clip: true

				// Don't stretch the image, and use asynchronous loading.
				fillMode: Image.PreserveAspectCrop
//TODO causes qmutex::lock on exit failure				asynchronous: true

				// Smoothing slows down the scrolling even more. Use it with consideration.
//				smooth: true
			}
		}

		// Reflection
		Item {
			width: delegateImage.width
			height: delegateImage.height

			Image {
				id: reflection

				width: delegateImage.width
				height: delegateImage.height
				anchors.centerIn: parent
				clip: true

				// The reflection uses the same image as the delegateImage.
				// This way there's no need to ask the image again from the provider.
				source: dlgImg.source
				opacity: 0.3

				fillMode: Image.PreserveAspectCrop
//TODO				asynchronous: true
//				smooth: true  // Use with consideration.

				transform : Scale {
					yScale: -1
					origin.y: delegateImage.height / 2
				}
			}
		}
	}

	// Rotation depends on the item's position on the PathView.
	// I.e. nicely rotate the image & reflection around Y-axis before disappearing.
	transform: Rotation {
		origin.x: delegateImage.width / 2
		origin.y: delegateImage.height / 2
		axis { x: 0; y: 1; z: 0 } // Rotate around y-axis.
		angle: delegateItem.pathAngle/1.5
	}

	// States and transitions for scaling the image.
	states: [
		State {
			name: "scaled"
			PropertyChanges {
				target: delegateImage
				// Scale up the icon
				scale: 1.8
			}
		}
	]

	transitions: [
		Transition {
			from: ""
			to: "scaled"
			reversible: true
			ParallelAnimation {
				PropertyAnimation {
					target: delegateImage
					properties: "scale"
					duration: 300
				}
			}
		}
	]

	MouseArea {
		anchors.fill: parent
		onClicked: coverFlow.currentIndex = index
	}
}
